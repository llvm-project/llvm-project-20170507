//===--- DumpAST.cpp ---------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Defines a few tweaks that expose AST and related information.
// Some of these are fairly clang-specific and hidden (e.g. textual AST dumps).
// Others are more generally useful (class layout) and are exposed by default.
//===----------------------------------------------------------------------===//
#include "refactor/Tweak.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Type.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/ScopedPrinter.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace clangd {
namespace {

/// Dumps the AST of the selected node.
/// Input:
///   fcall("foo");
///   ^^^^^
/// Message:
///   CallExpr
///   |-DeclRefExpr fcall
///   `-StringLiteral "foo"
class DumpAST : public Tweak {
public:
  const char *id() const override final;

  bool prepare(const Selection &Inputs) override {
    for (auto N = Inputs.ASTSelection.commonAncestor(); N && !Node;
         N = N->Parent)
      if (dumpable(N->ASTNode))
        Node = N->ASTNode;
    return Node.hasValue();
  }
  Expected<Effect> apply(const Selection &Inputs) override;
  std::string title() const override {
    return llvm::formatv("Dump {0} AST", Node->getNodeKind().asStringRef());
  }
  Intent intent() const override { return Info; }
  bool hidden() const override { return true; }

private:
  static bool dumpable(const ast_type_traits::DynTypedNode &N) {
    // Sadly not all node types can be dumped, and there's no API to check.
    // See DynTypedNode::dump().
    return N.get<Decl>() || N.get<Stmt>() || N.get<Type>();
  }

  llvm::Optional<ast_type_traits::DynTypedNode> Node;
};
REGISTER_TWEAK(DumpAST)

llvm::Expected<Tweak::Effect> DumpAST::apply(const Selection &Inputs) {
  std::string Str;
  llvm::raw_string_ostream OS(Str);
  Node->dump(OS, Inputs.AST.getASTContext().getSourceManager());
  return Effect::showMessage(std::move(OS.str()));
}

/// Dumps the SelectionTree.
/// Input:
/// int fcall(int);
/// void foo() {
///   fcall(2 + 2);
///     ^^^^^
/// }
/// Message:
/// TranslationUnitDecl
///   FunctionDecl void foo()
///     CompoundStmt {}
///      .CallExpr fcall(2 + 2)
///        ImplicitCastExpr fcall
///         .DeclRefExpr fcall
///        BinaryOperator 2 + 2
///          *IntegerLiteral 2
class ShowSelectionTree : public Tweak {
public:
  const char *id() const override final;

  bool prepare(const Selection &Inputs) override {
    return Inputs.ASTSelection.root() != nullptr;
  }
  Expected<Effect> apply(const Selection &Inputs) override {
    return Effect::showMessage(llvm::to_string(Inputs.ASTSelection));
  }
  std::string title() const override { return "Show selection tree"; }
  Intent intent() const override { return Info; }
  bool hidden() const override { return true; }
};
REGISTER_TWEAK(ShowSelectionTree);

/// Shows the layout of the RecordDecl under the cursor.
/// Input:
/// struct X { int foo; };
/// ^^^^^^^^
/// Message:
///        0 | struct S
///        0 |   int foo
///          | [sizeof=4, dsize=4, align=4,
///          |  nvsize=4, nvalign=4]
class DumpRecordLayout : public Tweak {
public:
  const char *id() const override final;

  bool prepare(const Selection &Inputs) override {
    if (auto *Node = Inputs.ASTSelection.commonAncestor())
      if (auto *D = Node->ASTNode.get<Decl>())
        Record = dyn_cast<RecordDecl>(D);
    return Record && Record->isThisDeclarationADefinition() &&
           !Record->isDependentType();
  }
  Expected<Effect> apply(const Selection &Inputs) override {
    std::string Str;
    llvm::raw_string_ostream OS(Str);
    Inputs.AST.getASTContext().DumpRecordLayout(Record, OS);
    return Effect::showMessage(std::move(OS.str()));
  }
  std::string title() const override {
    return llvm::formatv(
        "Show {0} layout",
        TypeWithKeyword::getTagTypeKindName(Record->getTagKind()));
  }
  Intent intent() const override { return Info; }

private:
  const RecordDecl *Record = nullptr;
};
REGISTER_TWEAK(DumpRecordLayout);

} // namespace
} // namespace clangd
} // namespace clang
