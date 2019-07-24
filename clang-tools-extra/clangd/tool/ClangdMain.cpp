//===--- ClangdMain.cpp - clangd server loop ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClangdLSPServer.h"
#include "CodeComplete.h"
#include "Features.inc"
#include "Path.h"
#include "Protocol.h"
#include "Trace.h"
#include "Transport.h"
#include "index/Background.h"
#include "index/Serialization.h"
#include "clang/Basic/Version.h"
#include "clang/Format/Format.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace clang {
namespace clangd {

using llvm::cl::cat;
using llvm::cl::CommaSeparated;
using llvm::cl::desc;
using llvm::cl::Hidden;
using llvm::cl::init;
using llvm::cl::list;
using llvm::cl::opt;
using llvm::cl::values;

static opt<Path> CompileCommandsDir{
    "compile-commands-dir",
    desc("Specify a path to look for compile_commands.json. If path "
         "is invalid, clangd will look in the current directory and "
         "parent paths of each source file"),
};

static opt<unsigned> WorkerThreadsCount{
    "j",
    desc("Number of async workers used by clangd"),
    init(getDefaultAsyncThreadsCount()),
};

// FIXME: also support "plain" style where signatures are always omitted.
enum CompletionStyleFlag { Detailed, Bundled };
static opt<CompletionStyleFlag> CompletionStyle{
    "completion-style",
    desc("Granularity of code completion suggestions"),
    values(clEnumValN(Detailed, "detailed",
                      "One completion item for each semantically distinct "
                      "completion, with full type information"),
           clEnumValN(Bundled, "bundled",
                      "Similar completion items (e.g. function overloads) are "
                      "combined. Type information shown where possible")),
};

// FIXME: Flags are the wrong mechanism for user preferences.
// We should probably read a dotfile or similar.
static opt<bool> IncludeIneligibleResults{
    "include-ineligible-results",
    desc("Include ineligible completion results (e.g. private members)"),
    init(CodeCompleteOptions().IncludeIneligibleResults),
    Hidden,
};

static opt<JSONStreamStyle> InputStyle{
    "input-style",
    desc("Input JSON stream encoding"),
    values(
        clEnumValN(JSONStreamStyle::Standard, "standard", "usual LSP protocol"),
        clEnumValN(JSONStreamStyle::Delimited, "delimited",
                   "messages delimited by --- lines, with # comment support")),
    init(JSONStreamStyle::Standard),
    Hidden,
};

static opt<bool> PrettyPrint{
    "pretty",
    desc("Pretty-print JSON output"),
    init(false),
};

static opt<Logger::Level> LogLevel{
    "log",
    desc("Verbosity of log messages written to stderr"),
    values(clEnumValN(Logger::Error, "error", "Error messages only"),
           clEnumValN(Logger::Info, "info", "High level execution tracing"),
           clEnumValN(Logger::Debug, "verbose", "Low level details")),
    init(Logger::Info),
};

static opt<bool> Test{
    "lit-test",
    desc("Abbreviation for -input-style=delimited -pretty -sync "
         "-enable-test-scheme -log=verbose."
         "Intended to simplify lit tests"),
    init(false),
    Hidden,
};

static opt<bool> EnableTestScheme{
    "enable-test-uri-scheme",
    desc("Enable 'test:' URI scheme. Only use in lit tests"),
    init(false),
    Hidden,
};

enum PCHStorageFlag { Disk, Memory };
static opt<PCHStorageFlag> PCHStorage{
    "pch-storage",
    desc("Storing PCHs in memory increases memory usages, but may "
         "improve performance"),
    values(
        clEnumValN(PCHStorageFlag::Disk, "disk", "store PCHs on disk"),
        clEnumValN(PCHStorageFlag::Memory, "memory", "store PCHs in memory")),
    init(PCHStorageFlag::Disk),
};

static opt<int> LimitResults{
    "limit-results",
    desc("Limit the number of results returned by clangd. "
         "0 means no limit (default=100)"),
    init(100),
};

static opt<bool> Sync{
    "sync",
    desc("Parse on main thread. If set, -j is ignored"),
    init(false),
    Hidden,
};

static opt<Path> ResourceDir{
    "resource-dir",
    desc("Directory for system clang headers"),
    init(""),
    Hidden,
};

static opt<Path> InputMirrorFile{
    "input-mirror-file",
    desc("Mirror all LSP input to the specified file. Useful for debugging"),
    init(""),
    Hidden,
};

static opt<bool> EnableIndex{
    "index",
    desc("Enable index-based features. By default, clangd maintains an index "
         "built from symbols in opened files. Global index support needs to "
         "enabled separatedly"),
    init(true),
    Hidden,
};

static opt<bool> AllScopesCompletion{
    "all-scopes-completion",
    desc("If set to true, code completion will include index symbols that are "
         "not defined in the scopes (e.g. "
         "namespaces) visible from the code completion point. Such completions "
         "can insert scope qualifiers"),
    init(true),
};

static opt<bool> ShowOrigins{
    "debug-origin",
    desc("Show origins of completion items"),
    init(CodeCompleteOptions().ShowOrigins),
    Hidden,
};

static opt<CodeCompleteOptions::IncludeInsertion> HeaderInsertion{
    "header-insertion",
    desc("Add #include directives when accepting code completions"),
    init(CodeCompleteOptions().InsertIncludes),
    values(
        clEnumValN(CodeCompleteOptions::IWYU, "iwyu",
                   "Include what you use. "
                   "Insert the owning header for top-level symbols, unless the "
                   "header is already directly included or the symbol is "
                   "forward-declared"),
        clEnumValN(
            CodeCompleteOptions::NeverInsert, "never",
            "Never insert #include directives as part of code completion")),
};

static opt<bool> HeaderInsertionDecorators{
    "header-insertion-decorators",
    desc("Prepend a circular dot or space before the completion "
         "label, depending on whether "
         "an include line will be inserted or not"),
    init(true),
};

static opt<Path> IndexFile{
    "index-file",
    desc(
        "Index file to build the static index. The file must have been created "
        "by a compatible clangd-indexer\n"
        "WARNING: This option is experimental only, and will be removed "
        "eventually. Don't rely on it"),
    init(""),
    Hidden,
};

static opt<bool> EnableBackgroundIndex{
    "background-index",
    desc("Index project code in the background and persist index on disk. "
         "Experimental"),
    init(true),
};

enum CompileArgsFrom { LSPCompileArgs, FilesystemCompileArgs };
static opt<CompileArgsFrom> CompileArgsFrom{
    "compile_args_from",
    desc("The source of compile commands"),
    values(clEnumValN(LSPCompileArgs, "lsp",
                      "All compile commands come from LSP and "
                      "'compile_commands.json' files are ignored"),
           clEnumValN(FilesystemCompileArgs, "filesystem",
                      "All compile commands come from the "
                      "'compile_commands.json' files")),
    init(FilesystemCompileArgs),
    Hidden,
};

static opt<bool> EnableFunctionArgSnippets{
    "function-arg-placeholders",
    desc("When disabled, completions contain only parentheses for "
         "function calls. When enabled, completions also contain "
         "placeholders for method parameters"),
    init(CodeCompleteOptions().EnableFunctionArgSnippets),
    Hidden,
};

static opt<std::string> ClangTidyChecks{
    "clang-tidy-checks",
    desc("List of clang-tidy checks to run (this will override "
         ".clang-tidy files). Only meaningful when -clang-tidy flag is on"),
    init(""),
};

static opt<bool> EnableClangTidy{
    "clang-tidy",
    desc("Enable clang-tidy diagnostics"),
    init(true),
};

static opt<std::string> FallbackStyle{
    "fallback-style",
    desc("clang-format style to apply by default when "
         "no .clang-format file is found"),
    init(clang::format::DefaultFallbackStyle),
};

static opt<bool> SuggestMissingIncludes{
    "suggest-missing-includes",
    desc("Attempts to fix diagnostic errors caused by missing "
         "includes using index"),
    init(true),
};

static opt<OffsetEncoding> ForceOffsetEncoding{
    "offset-encoding",
    desc("Force the offsetEncoding used for character positions. "
         "This bypasses negotiation via client capabilities"),
    values(
        clEnumValN(OffsetEncoding::UTF8, "utf-8", "Offsets are in UTF-8 bytes"),
        clEnumValN(OffsetEncoding::UTF16, "utf-16",
                   "Offsets are in UTF-16 code units")),
    init(OffsetEncoding::UnsupportedEncoding),
};

static opt<CodeCompleteOptions::CodeCompletionParse> CodeCompletionParse{
    "completion-parse",
    desc("Whether the clang-parser is used for code-completion"),
    values(clEnumValN(CodeCompleteOptions::AlwaysParse, "always",
                      "Block until the parser can be used"),
           clEnumValN(CodeCompleteOptions::ParseIfReady, "auto",
                      "Use text-based completion if the parser "
                      "is not ready"),
           clEnumValN(CodeCompleteOptions::NeverParse, "never",
                      "Always used text-based completion")),
    init(CodeCompleteOptions().RunParser),
    Hidden,
};

static opt<bool> HiddenFeatures{
    "hidden-features",
    desc("Enable hidden features mostly useful to clangd developers"),
    init(false),
    Hidden,
};

static list<std::string> QueryDriverGlobs{
    "query-driver",
    desc(
        "Comma separated list of globs for white-listing gcc-compatible "
        "drivers that are safe to execute. Drivers matching any of these globs "
        "will be used to extract system includes. e.g. "
        "/usr/bin/**/clang-*,/path/to/repo/**/g++-*"),
    CommaSeparated,
};

static list<std::string> TweakList{
    "tweaks",
    desc("Specify a list of Tweaks to enable (only for clangd developers)."),
    Hidden,
    CommaSeparated,
};

namespace {

/// \brief Supports a test URI scheme with relaxed constraints for lit tests.
/// The path in a test URI will be combined with a platform-specific fake
/// directory to form an absolute path. For example, test:///a.cpp is resolved
/// C:\clangd-test\a.cpp on Windows and /clangd-test/a.cpp on Unix.
class TestScheme : public URIScheme {
public:
  llvm::Expected<std::string>
  getAbsolutePath(llvm::StringRef /*Authority*/, llvm::StringRef Body,
                  llvm::StringRef /*HintPath*/) const override {
    using namespace llvm::sys;
    // Still require "/" in body to mimic file scheme, as we want lengths of an
    // equivalent URI in both schemes to be the same.
    if (!Body.startswith("/"))
      return llvm::make_error<llvm::StringError>(
          "Expect URI body to be an absolute path starting with '/': " + Body,
          llvm::inconvertibleErrorCode());
    Body = Body.ltrim('/');
    llvm::SmallVector<char, 16> Path(Body.begin(), Body.end());
    path::native(Path);
    fs::make_absolute(TestScheme::TestDir, Path);
    return std::string(Path.begin(), Path.end());
  }

  llvm::Expected<URI>
  uriFromAbsolutePath(llvm::StringRef AbsolutePath) const override {
    llvm::StringRef Body = AbsolutePath;
    if (!Body.consume_front(TestScheme::TestDir)) {
      return llvm::make_error<llvm::StringError>(
          "Path " + AbsolutePath + " doesn't start with root " + TestDir,
          llvm::inconvertibleErrorCode());
    }

    return URI("test", /*Authority=*/"",
               llvm::sys::path::convert_to_slash(Body));
  }

private:
  const static char TestDir[];
};

#ifdef _WIN32
const char TestScheme::TestDir[] = "C:\\clangd-test";
#else
const char TestScheme::TestDir[] = "/clangd-test";
#endif

} // namespace
} // namespace clangd
} // namespace clang

enum class ErrorResultCode : int {
  NoShutdownRequest = 1,
  CantRunAsXPCService = 2
};

int main(int argc, char *argv[]) {
  using namespace clang;
  using namespace clang::clangd;

  llvm::InitializeAllTargetInfos();
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {
    OS << clang::getClangToolFullVersion("clangd") << "\n";
  });
  llvm::cl::ParseCommandLineOptions(
      argc, argv,
      "clangd is a language server that provides IDE-like features to editors. "
      "\n\nIt should be used via an editor plugin rather than invoked "
      "directly. "
      "For more information, see:"
      "\n\thttps://clang.llvm.org/extra/clangd.html"
      "\n\thttps://microsoft.github.io/language-server-protocol/");
  if (Test) {
    Sync = true;
    InputStyle = JSONStreamStyle::Delimited;
    LogLevel = Logger::Verbose;
    PrettyPrint = true;
    // Disable background index on lit tests by default to prevent disk writes.
    if (!EnableBackgroundIndex.getNumOccurrences())
      EnableBackgroundIndex = false;
    // Ensure background index makes progress.
    else if (EnableBackgroundIndex)
      BackgroundQueue::preventThreadStarvationInTests();
  }
  if (Test || EnableTestScheme) {
    static URISchemeRegistry::Add<TestScheme> X(
        "test", "Test scheme for clangd lit tests.");
  }

  if (!Sync && WorkerThreadsCount == 0) {
    llvm::errs() << "A number of worker threads cannot be 0. Did you mean to "
                    "specify -sync?";
    return 1;
  }

  if (Sync) {
    if (WorkerThreadsCount.getNumOccurrences())
      llvm::errs() << "Ignoring -j because -sync is set.\n";
    WorkerThreadsCount = 0;
  }
  if (FallbackStyle.getNumOccurrences())
    clang::format::DefaultFallbackStyle = FallbackStyle.c_str();

  // Validate command line arguments.
  llvm::Optional<llvm::raw_fd_ostream> InputMirrorStream;
  if (!InputMirrorFile.empty()) {
    std::error_code EC;
    InputMirrorStream.emplace(InputMirrorFile, /*ref*/ EC,
                              llvm::sys::fs::FA_Read | llvm::sys::fs::FA_Write);
    if (EC) {
      InputMirrorStream.reset();
      llvm::errs() << "Error while opening an input mirror file: "
                   << EC.message();
    } else {
      InputMirrorStream->SetUnbuffered();
    }
  }

  // Setup tracing facilities if CLANGD_TRACE is set. In practice enabling a
  // trace flag in your editor's config is annoying, launching with
  // `CLANGD_TRACE=trace.json vim` is easier.
  llvm::Optional<llvm::raw_fd_ostream> TraceStream;
  std::unique_ptr<trace::EventTracer> Tracer;
  if (auto *TraceFile = getenv("CLANGD_TRACE")) {
    std::error_code EC;
    TraceStream.emplace(TraceFile, /*ref*/ EC,
                        llvm::sys::fs::FA_Read | llvm::sys::fs::FA_Write);
    if (EC) {
      TraceStream.reset();
      llvm::errs() << "Error while opening trace file " << TraceFile << ": "
                   << EC.message();
    } else {
      Tracer = trace::createJSONTracer(*TraceStream, PrettyPrint);
    }
  }

  llvm::Optional<trace::Session> TracingSession;
  if (Tracer)
    TracingSession.emplace(*Tracer);

  // Use buffered stream to stderr (we still flush each log message). Unbuffered
  // stream can cause significant (non-deterministic) latency for the logger.
  llvm::errs().SetBuffered();
  StreamLogger Logger(llvm::errs(), LogLevel);
  LoggingSession LoggingSession(Logger);
  // Write some initial logs before we start doing any real work.
  log("{0}", clang::getClangToolFullVersion("clangd"));
  {
    SmallString<128> CWD;
    if (auto Err = llvm::sys::fs::current_path(CWD))
      log("Working directory unknown: {0}", Err.message());
    else
      log("Working directory: {0}", CWD);
  }
  for (int I = 0; I < argc; ++I)
    log("argv[{0}]: {1}", I, argv[I]);

  // If --compile-commands-dir arg was invoked, check value and override default
  // path.
  llvm::Optional<Path> CompileCommandsDirPath;
  if (!CompileCommandsDir.empty()) {
    if (llvm::sys::fs::exists(CompileCommandsDir)) {
      // We support passing both relative and absolute paths to the
      // --compile-commands-dir argument, but we assume the path is absolute in
      // the rest of clangd so we make sure the path is absolute before
      // continuing.
      llvm::SmallString<128> Path(CompileCommandsDir);
      if (std::error_code EC = llvm::sys::fs::make_absolute(Path)) {
        llvm::errs() << "Error while converting the relative path specified by "
                        "--compile-commands-dir to an absolute path: "
                     << EC.message() << ". The argument will be ignored.\n";
      } else {
        CompileCommandsDirPath = Path.str();
      }
    } else {
      llvm::errs()
          << "Path specified by --compile-commands-dir does not exist. The "
             "argument will be ignored.\n";
    }
  }

  ClangdServer::Options Opts;
  switch (PCHStorage) {
  case PCHStorageFlag::Memory:
    Opts.StorePreamblesInMemory = true;
    break;
  case PCHStorageFlag::Disk:
    Opts.StorePreamblesInMemory = false;
    break;
  }
  if (!ResourceDir.empty())
    Opts.ResourceDir = ResourceDir;
  Opts.BuildDynamicSymbolIndex = EnableIndex;
  Opts.BackgroundIndex = EnableBackgroundIndex;
  std::unique_ptr<SymbolIndex> StaticIdx;
  std::future<void> AsyncIndexLoad; // Block exit while loading the index.
  if (EnableIndex && !IndexFile.empty()) {
    // Load the index asynchronously. Meanwhile SwapIndex returns no results.
    SwapIndex *Placeholder;
    StaticIdx.reset(Placeholder = new SwapIndex(llvm::make_unique<MemIndex>()));
    AsyncIndexLoad = runAsync<void>([Placeholder] {
      if (auto Idx = loadIndex(IndexFile, /*UseDex=*/true))
        Placeholder->reset(std::move(Idx));
    });
    if (Sync)
      AsyncIndexLoad.wait();
  }
  Opts.StaticIndex = StaticIdx.get();
  Opts.AsyncThreadsCount = WorkerThreadsCount;

  clangd::CodeCompleteOptions CCOpts;
  CCOpts.IncludeIneligibleResults = IncludeIneligibleResults;
  CCOpts.Limit = LimitResults;
  if (CompletionStyle.getNumOccurrences())
    CCOpts.BundleOverloads = CompletionStyle != Detailed;
  CCOpts.ShowOrigins = ShowOrigins;
  CCOpts.InsertIncludes = HeaderInsertion;
  if (!HeaderInsertionDecorators) {
    CCOpts.IncludeIndicator.Insert.clear();
    CCOpts.IncludeIndicator.NoInsert.clear();
  }
  CCOpts.SpeculativeIndexRequest = Opts.StaticIndex;
  CCOpts.EnableFunctionArgSnippets = EnableFunctionArgSnippets;
  CCOpts.AllScopes = AllScopesCompletion;
  CCOpts.RunParser = CodeCompletionParse;

  RealFileSystemProvider FSProvider;
  // Initialize and run ClangdLSPServer.
  // Change stdin to binary to not lose \r\n on windows.
  llvm::sys::ChangeStdinToBinary();

  std::unique_ptr<Transport> TransportLayer;
  if (getenv("CLANGD_AS_XPC_SERVICE")) {
#if CLANGD_BUILD_XPC
    log("Starting LSP over XPC service");
    TransportLayer = newXPCTransport();
#else
    llvm::errs() << "This clangd binary wasn't built with XPC support.\n";
    return (int)ErrorResultCode::CantRunAsXPCService;
#endif
  } else {
    log("Starting LSP over stdin/stdout");
    TransportLayer = newJSONTransport(
        stdin, llvm::outs(),
        InputMirrorStream ? InputMirrorStream.getPointer() : nullptr,
        PrettyPrint, InputStyle);
  }

  // Create an empty clang-tidy option.
  std::mutex ClangTidyOptMu;
  std::unique_ptr<tidy::ClangTidyOptionsProvider>
      ClangTidyOptProvider; /*GUARDED_BY(ClangTidyOptMu)*/
  if (EnableClangTidy) {
    auto OverrideClangTidyOptions = tidy::ClangTidyOptions::getDefaults();
    OverrideClangTidyOptions.Checks = ClangTidyChecks;
    ClangTidyOptProvider = llvm::make_unique<tidy::FileOptionsProvider>(
        tidy::ClangTidyGlobalOptions(),
        /* Default */ tidy::ClangTidyOptions::getDefaults(),
        /* Override */ OverrideClangTidyOptions, FSProvider.getFileSystem());
    Opts.GetClangTidyOptions = [&](llvm::vfs::FileSystem &,
                                   llvm::StringRef File) {
      // This function must be thread-safe and tidy option providers are not.
      std::lock_guard<std::mutex> Lock(ClangTidyOptMu);
      // FIXME: use the FS provided to the function.
      return ClangTidyOptProvider->getOptions(File);
    };
  }
  Opts.SuggestMissingIncludes = SuggestMissingIncludes;
  Opts.QueryDriverGlobs = std::move(QueryDriverGlobs);

  Opts.TweakFilter = [&](const Tweak &T) {
    if (T.hidden() && !HiddenFeatures)
      return false;
    if (TweakList.getNumOccurrences())
      return llvm::is_contained(TweakList, T.id());
    return true;
  };
  llvm::Optional<OffsetEncoding> OffsetEncodingFromFlag;
  if (ForceOffsetEncoding != OffsetEncoding::UnsupportedEncoding)
    OffsetEncodingFromFlag = ForceOffsetEncoding;
  ClangdLSPServer LSPServer(
      *TransportLayer, FSProvider, CCOpts, CompileCommandsDirPath,
      /*UseDirBasedCDB=*/CompileArgsFrom == FilesystemCompileArgs,
      OffsetEncodingFromFlag, Opts);
  llvm::set_thread_name("clangd.main");
  return LSPServer.run() ? 0
                         : static_cast<int>(ErrorResultCode::NoShutdownRequest);
}
