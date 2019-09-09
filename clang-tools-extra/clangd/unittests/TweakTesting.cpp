//===-- TweakTesting.cpp ------------------------------------------------*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TweakTesting.h"

#include "Annotations.h"
#include "SourceCode.h"
#include "refactor/Tweak.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace clangd {
namespace {
using Context = TweakTest::CodeContext;

std::pair<llvm::StringRef, llvm::StringRef> wrapping(Context Ctx) {
  switch (Ctx) {
    case TweakTest::File:
      return {"",""};
    case TweakTest::Function:
      return {"void wrapperFunction(){\n", "\n}"};
    case TweakTest::Expression:
      return {"auto expressionWrapper(){return\n", "\n;}"};
  }
  llvm_unreachable("Unknown TweakTest::CodeContext enum");
}

std::string wrap(Context Ctx, llvm::StringRef Inner) {
  auto Wrapping = wrapping(Ctx);
  return (Wrapping.first + Inner + Wrapping.second).str();
}

llvm::StringRef unwrap(Context Ctx, llvm::StringRef Outer) {
  auto Wrapping = wrapping(Ctx);
  // Unwrap only if the code matches the expected wrapping.
  // Don't allow the begin/end wrapping to overlap!
  if (Outer.startswith(Wrapping.first) && Outer.endswith(Wrapping.second) &&
      Outer.size() >= Wrapping.first.size() + Wrapping.second.size())
    return Outer.drop_front(Wrapping.first.size()).drop_back(Wrapping.second.size());
  return Outer;
}

std::pair<unsigned, unsigned> rangeOrPoint(const Annotations &A) {
  Range SelectionRng;
  if (A.points().size() != 0) {
    assert(A.ranges().size() == 0 &&
           "both a cursor point and a selection range were specified");
    SelectionRng = Range{A.point(), A.point()};
  } else {
    SelectionRng = A.range();
  }
  return {cantFail(positionToOffset(A.code(), SelectionRng.start)),
          cantFail(positionToOffset(A.code(), SelectionRng.end))};
}

MATCHER_P3(TweakIsAvailable, TweakID, Ctx, Header,
           (TweakID + (negation ? " is unavailable" : " is available")).str()) {
  std::string WrappedCode = wrap(Ctx, arg);
  Annotations Input(WrappedCode);
  auto Selection = rangeOrPoint(Input);
  TestTU TU;
  TU.HeaderCode = Header;
  TU.Code = Input.code();
  ParsedAST AST = TU.build();
  Tweak::Selection S(AST, Selection.first, Selection.second);
  auto PrepareResult = prepareTweak(TweakID, S);
  if (PrepareResult)
    return true;
  llvm::consumeError(PrepareResult.takeError());
  return false;
}

} // namespace

std::string TweakTest::apply(llvm::StringRef MarkedCode) const {
  std::string WrappedCode = wrap(Context, MarkedCode);
  Annotations Input(WrappedCode);
  auto Selection = rangeOrPoint(Input);
  TestTU TU;
  TU.HeaderCode = Header;
  TU.Code = Input.code();
  ParsedAST AST = TU.build();
  Tweak::Selection S(AST, Selection.first, Selection.second);

  auto T = prepareTweak(TweakID, S);
  if (!T) {
    llvm::consumeError(T.takeError());
    return "unavailable";
  }
  llvm::Expected<Tweak::Effect> Result = (*T)->apply(S);
  if (!Result)
    return "fail: " + llvm::toString(Result.takeError());
  if (Result->ShowMessage)
    return "message:\n" + *Result->ShowMessage;
  if (Result->ApplyEdits.empty())
    return "no effect";
  if (Result->ApplyEdits.size() > 1)
    return "received multi-file edits";

  auto ApplyEdit = Result->ApplyEdits.begin()->second;
  if (auto NewText = ApplyEdit.apply())
    return unwrap(Context, *NewText);
  else
    return "bad edits: " + llvm::toString(NewText.takeError());
}

::testing::Matcher<llvm::StringRef> TweakTest::isAvailable() const {
  return TweakIsAvailable(llvm::StringRef(TweakID), Context, Header); 
}

std::vector<std::string> TweakTest::expandCases(llvm::StringRef MarkedCode) {
  Annotations Test(MarkedCode);
  llvm::StringRef Code = Test.code();
  std::vector<std::string> Cases;
  for (const auto& Point : Test.points()) {
    size_t Offset = llvm::cantFail(positionToOffset(Code, Point));
    Cases.push_back((Code.substr(0, Offset) + "^" + Code.substr(Offset)).str());
  }
  for (const auto& Range : Test.ranges()) {
    size_t Begin = llvm::cantFail(positionToOffset(Code, Range.start));
    size_t End = llvm::cantFail(positionToOffset(Code, Range.end));
    Cases.push_back((Code.substr(0, Begin) + "[[" +
                     Code.substr(Begin, End - Begin) + "]]" + Code.substr(End))
                        .str());
  }
  assert(!Cases.empty() && "No markings in MarkedCode?");
  return Cases;
}

} // namespace clangd
} // namespace clang
