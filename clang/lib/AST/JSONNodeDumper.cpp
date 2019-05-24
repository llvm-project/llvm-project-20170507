#include "clang/AST/JSONNodeDumper.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;

void JSONNodeDumper::addPreviousDeclaration(const Decl *D) {
  switch (D->getKind()) {
#define DECL(DERIVED, BASE)                                                    \
  case Decl::DERIVED:                                                          \
    return writePreviousDeclImpl(cast<DERIVED##Decl>(D));
#define ABSTRACT_DECL(DECL)
#include "clang/AST/DeclNodes.inc"
#undef ABSTRACT_DECL
#undef DECL
  }
  llvm_unreachable("Decl that isn't part of DeclNodes.inc!");
}

void JSONNodeDumper::Visit(const Attr *A) {
  const char *AttrName = nullptr;
  switch (A->getKind()) {
#define ATTR(X)                                                                \
  case attr::X:                                                                \
    AttrName = #X"Attr";                                                       \
    break;
#include "clang/Basic/AttrList.inc"
#undef ATTR
  }
  JOS.attribute("id", createPointerRepresentation(A));
  JOS.attribute("kind", AttrName);
  JOS.attribute("range", createSourceRange(A->getRange()));
  attributeOnlyIfTrue("inherited", A->isInherited());
  attributeOnlyIfTrue("implicit", A->isImplicit());

  // FIXME: it would be useful for us to output the spelling kind as well as
  // the actual spelling. This would allow us to distinguish between the
  // various attribute syntaxes, but we don't currently track that information
  // within the AST.
  //JOS.attribute("spelling", A->getSpelling());

  InnerAttrVisitor::Visit(A);
}

void JSONNodeDumper::Visit(const Stmt *S) {
  if (!S)
    return;

  JOS.attribute("id", createPointerRepresentation(S));
  JOS.attribute("kind", S->getStmtClassName());
  JOS.attribute("range", createSourceRange(S->getSourceRange()));

  if (const auto *E = dyn_cast<Expr>(S)) {
    JOS.attribute("type", createQualType(E->getType()));
    const char *Category = nullptr;
    switch (E->getValueKind()) {
    case VK_LValue: Category = "lvalue"; break;
    case VK_XValue: Category = "xvalue"; break;
    case VK_RValue: Category = "rvalue"; break;
    }
    JOS.attribute("valueCategory", Category);
  }
  InnerStmtVisitor::Visit(S);
}

void JSONNodeDumper::Visit(const Type *T) {
  JOS.attribute("id", createPointerRepresentation(T));
  JOS.attribute("kind", (llvm::Twine(T->getTypeClassName()) + "Type").str());
  JOS.attribute("type", createQualType(QualType(T, 0), /*Desugar*/ false));
  attributeOnlyIfTrue("isDependent", T->isDependentType());
  attributeOnlyIfTrue("isInstantiationDependent",
                      T->isInstantiationDependentType());
  attributeOnlyIfTrue("isVariablyModified", T->isVariablyModifiedType());
  attributeOnlyIfTrue("containsUnexpandedPack",
                      T->containsUnexpandedParameterPack());
  attributeOnlyIfTrue("isImported", T->isFromAST());
  InnerTypeVisitor::Visit(T);
}

void JSONNodeDumper::Visit(QualType T) {
  JOS.attribute("id", createPointerRepresentation(T.getAsOpaquePtr()));
  JOS.attribute("type", createQualType(T));
  JOS.attribute("qualifiers", T.split().Quals.getAsString());
}

void JSONNodeDumper::Visit(const Decl *D) {
  JOS.attribute("id", createPointerRepresentation(D));

  if (!D)
    return;

  JOS.attribute("kind", (llvm::Twine(D->getDeclKindName()) + "Decl").str());
  JOS.attribute("loc", createSourceLocation(D->getLocation()));
  JOS.attribute("range", createSourceRange(D->getSourceRange()));
  attributeOnlyIfTrue("isImplicit", D->isImplicit());
  attributeOnlyIfTrue("isInvalid", D->isInvalidDecl());

  if (D->isUsed())
    JOS.attribute("isUsed", true);
  else if (D->isThisDeclarationReferenced())
    JOS.attribute("isReferenced", true);

  if (const auto *ND = dyn_cast<NamedDecl>(D))
    attributeOnlyIfTrue("isHidden", ND->isHidden());

  if (D->getLexicalDeclContext() != D->getDeclContext())
    JOS.attribute("parentDeclContext",
                  createPointerRepresentation(D->getDeclContext()));

  addPreviousDeclaration(D);
  InnerDeclVisitor::Visit(D);
}

void JSONNodeDumper::Visit(const comments::Comment *C,
                           const comments::FullComment *FC) {
  if (!C)
    return;

  JOS.attribute("id", createPointerRepresentation(C));
  JOS.attribute("kind", C->getCommentKindName());
  JOS.attribute("loc", createSourceLocation(C->getLocation()));
  JOS.attribute("range", createSourceRange(C->getSourceRange()));

  InnerCommentVisitor::visit(C, FC);
}

void JSONNodeDumper::Visit(const TemplateArgument &TA, SourceRange R,
                           const Decl *From, StringRef Label) {
  JOS.attribute("kind", "TemplateArgument");
  if (R.isValid())
    JOS.attribute("range", createSourceRange(R));

  if (From)
    JOS.attribute(Label.empty() ? "fromDecl" : Label, createBareDeclRef(From));

  InnerTemplateArgVisitor::Visit(TA);
}

void JSONNodeDumper::Visit(const CXXCtorInitializer *Init) {
  JOS.attribute("kind", "CXXCtorInitializer");
  if (Init->isAnyMemberInitializer())
    JOS.attribute("anyInit", createBareDeclRef(Init->getAnyMember()));
  else if (Init->isBaseInitializer())
    JOS.attribute("baseInit",
                  createQualType(QualType(Init->getBaseClass(), 0)));
  else if (Init->isDelegatingInitializer())
    JOS.attribute("delegatingInit",
                  createQualType(Init->getTypeSourceInfo()->getType()));
  else
    llvm_unreachable("Unknown initializer type");
}

void JSONNodeDumper::Visit(const OMPClause *C) {}

void JSONNodeDumper::Visit(const BlockDecl::Capture &C) {
  JOS.attribute("kind", "Capture");
  attributeOnlyIfTrue("byref", C.isByRef());
  attributeOnlyIfTrue("nested", C.isNested());
  if (C.getVariable())
    JOS.attribute("var", createBareDeclRef(C.getVariable()));
}

void JSONNodeDumper::Visit(const GenericSelectionExpr::ConstAssociation &A) {
  JOS.attribute("associationKind", A.getTypeSourceInfo() ? "case" : "default");
  attributeOnlyIfTrue("selected", A.isSelected());
}

llvm::json::Object
JSONNodeDumper::createBareSourceLocation(SourceLocation Loc) {
  PresumedLoc Presumed = SM.getPresumedLoc(Loc);

  if (Presumed.isInvalid())
    return llvm::json::Object{};

  return llvm::json::Object{{"file", Presumed.getFilename()},
                            {"line", Presumed.getLine()},
                            {"col", Presumed.getColumn()}};
}

llvm::json::Object JSONNodeDumper::createSourceLocation(SourceLocation Loc) {
  SourceLocation Spelling = SM.getSpellingLoc(Loc);
  SourceLocation Expansion = SM.getExpansionLoc(Loc);

  llvm::json::Object SLoc = createBareSourceLocation(Spelling);
  if (Expansion != Spelling) {
    // If the expansion and the spelling are different, output subobjects
    // describing both locations.
    llvm::json::Object ELoc = createBareSourceLocation(Expansion);

    // If there is a macro expansion, add extra information if the interesting
    // bit is the macro arg expansion.
    if (SM.isMacroArgExpansion(Loc))
      ELoc["isMacroArgExpansion"] = true;

    return llvm::json::Object{{"spellingLoc", std::move(SLoc)},
                              {"expansionLoc", std::move(ELoc)}};
  }

  return SLoc;
}

llvm::json::Object JSONNodeDumper::createSourceRange(SourceRange R) {
  return llvm::json::Object{{"begin", createSourceLocation(R.getBegin())},
                            {"end", createSourceLocation(R.getEnd())}};
}

std::string JSONNodeDumper::createPointerRepresentation(const void *Ptr) {
  // Because JSON stores integer values as signed 64-bit integers, trying to
  // represent them as such makes for very ugly pointer values in the resulting
  // output. Instead, we convert the value to hex and treat it as a string.
  return "0x" + llvm::utohexstr(reinterpret_cast<uint64_t>(Ptr), true);
}

llvm::json::Object JSONNodeDumper::createQualType(QualType QT, bool Desugar) {
  SplitQualType SQT = QT.split();
  llvm::json::Object Ret{{"qualType", QualType::getAsString(SQT, PrintPolicy)}};

  if (Desugar && !QT.isNull()) {
    SplitQualType DSQT = QT.getSplitDesugaredType();
    if (DSQT != SQT)
      Ret["desugaredQualType"] = QualType::getAsString(DSQT, PrintPolicy);
  }
  return Ret;
}

llvm::json::Object JSONNodeDumper::createBareDeclRef(const Decl *D) {
  llvm::json::Object Ret{{"id", createPointerRepresentation(D)}};
  if (!D)
    return Ret;

  Ret["kind"] = (llvm::Twine(D->getDeclKindName()) + "Decl").str();
  if (const auto *ND = dyn_cast<NamedDecl>(D))
    Ret["name"] = ND->getDeclName().getAsString();
  if (const auto *VD = dyn_cast<ValueDecl>(D))
    Ret["type"] = createQualType(VD->getType());
  return Ret;
}

llvm::json::Array JSONNodeDumper::createCastPath(const CastExpr *C) {
  llvm::json::Array Ret;
  if (C->path_empty())
    return Ret;

  for (auto I = C->path_begin(), E = C->path_end(); I != E; ++I) {
    const CXXBaseSpecifier *Base = *I;
    const auto *RD =
        cast<CXXRecordDecl>(Base->getType()->getAs<RecordType>()->getDecl());

    llvm::json::Object Val{{"name", RD->getName()}};
    if (Base->isVirtual())
      Val["isVirtual"] = true;
    Ret.push_back(std::move(Val));
  }
  return Ret;
}

#define FIELD2(Name, Flag)  if (RD->Flag()) Ret[Name] = true
#define FIELD1(Flag)        FIELD2(#Flag, Flag)

static llvm::json::Object
createDefaultConstructorDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("exists", hasDefaultConstructor);
  FIELD2("trivial", hasTrivialDefaultConstructor);
  FIELD2("nonTrivial", hasNonTrivialDefaultConstructor);
  FIELD2("userProvided", hasUserProvidedDefaultConstructor);
  FIELD2("isConstexpr", hasConstexprDefaultConstructor);
  FIELD2("needsImplicit", needsImplicitDefaultConstructor);
  FIELD2("defaultedIsConstexpr", defaultedDefaultConstructorIsConstexpr);

  return Ret;
}

static llvm::json::Object
createCopyConstructorDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("simple", hasSimpleCopyConstructor);
  FIELD2("trivial", hasTrivialCopyConstructor);
  FIELD2("nonTrivial", hasNonTrivialCopyConstructor);
  FIELD2("userDeclared", hasUserDeclaredCopyConstructor);
  FIELD2("hasConstParam", hasCopyConstructorWithConstParam);
  FIELD2("implicitHasConstParam", implicitCopyConstructorHasConstParam);
  FIELD2("needsImplicit", needsImplicitCopyConstructor);
  FIELD2("needsOverloadResolution", needsOverloadResolutionForCopyConstructor);
  if (!RD->needsOverloadResolutionForCopyConstructor())
    FIELD2("defaultedIsDeleted", defaultedCopyConstructorIsDeleted);

  return Ret;
}

static llvm::json::Object
createMoveConstructorDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("exists", hasMoveConstructor);
  FIELD2("simple", hasSimpleMoveConstructor);
  FIELD2("trivial", hasTrivialMoveConstructor);
  FIELD2("nonTrivial", hasNonTrivialMoveConstructor);
  FIELD2("userDeclared", hasUserDeclaredMoveConstructor);
  FIELD2("needsImplicit", needsImplicitMoveConstructor);
  FIELD2("needsOverloadResolution", needsOverloadResolutionForMoveConstructor);
  if (!RD->needsOverloadResolutionForMoveConstructor())
    FIELD2("defaultedIsDeleted", defaultedMoveConstructorIsDeleted);

  return Ret;
}

static llvm::json::Object
createCopyAssignmentDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("trivial", hasTrivialCopyAssignment);
  FIELD2("nonTrivial", hasNonTrivialCopyAssignment);
  FIELD2("hasConstParam", hasCopyAssignmentWithConstParam);
  FIELD2("implicitHasConstParam", implicitCopyAssignmentHasConstParam);
  FIELD2("userDeclared", hasUserDeclaredCopyAssignment);
  FIELD2("needsImplicit", needsImplicitCopyAssignment);
  FIELD2("needsOverloadResolution", needsOverloadResolutionForCopyAssignment);

  return Ret;
}

static llvm::json::Object
createMoveAssignmentDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("exists", hasMoveAssignment);
  FIELD2("simple", hasSimpleMoveAssignment);
  FIELD2("trivial", hasTrivialMoveAssignment);
  FIELD2("nonTrivial", hasNonTrivialMoveAssignment);
  FIELD2("userDeclared", hasUserDeclaredMoveAssignment);
  FIELD2("needsImplicit", needsImplicitMoveAssignment);
  FIELD2("needsOverloadResolution", needsOverloadResolutionForMoveAssignment);

  return Ret;
}

static llvm::json::Object
createDestructorDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  FIELD2("simple", hasSimpleDestructor);
  FIELD2("irrelevant", hasIrrelevantDestructor);
  FIELD2("trivial", hasTrivialDestructor);
  FIELD2("nonTrivial", hasNonTrivialDestructor);
  FIELD2("userDeclared", hasUserDeclaredDestructor);
  FIELD2("needsImplicit", needsImplicitDestructor);
  FIELD2("needsOverloadResolution", needsOverloadResolutionForDestructor);
  if (!RD->needsOverloadResolutionForDestructor())
    FIELD2("defaultedIsDeleted", defaultedDestructorIsDeleted);

  return Ret;
}

llvm::json::Object
JSONNodeDumper::createCXXRecordDefinitionData(const CXXRecordDecl *RD) {
  llvm::json::Object Ret;

  // This data is common to all C++ classes.
  FIELD1(isGenericLambda);
  FIELD1(isLambda);
  FIELD1(isEmpty);
  FIELD1(isAggregate);
  FIELD1(isStandardLayout);
  FIELD1(isTriviallyCopyable);
  FIELD1(isPOD);
  FIELD1(isTrivial);
  FIELD1(isPolymorphic);
  FIELD1(isAbstract);
  FIELD1(isLiteral);
  FIELD1(canPassInRegisters);
  FIELD1(hasUserDeclaredConstructor);
  FIELD1(hasConstexprNonCopyMoveConstructor);
  FIELD1(hasMutableFields);
  FIELD1(hasVariantMembers);
  FIELD2("canConstDefaultInit", allowConstDefaultInit);

  Ret["defaultCtor"] = createDefaultConstructorDefinitionData(RD);
  Ret["copyCtor"] = createCopyConstructorDefinitionData(RD);
  Ret["moveCtor"] = createMoveConstructorDefinitionData(RD);
  Ret["copyAssign"] = createCopyAssignmentDefinitionData(RD);
  Ret["moveAssign"] = createMoveAssignmentDefinitionData(RD);
  Ret["dtor"] = createDestructorDefinitionData(RD);

  return Ret;
}

#undef FIELD1
#undef FIELD2

std::string JSONNodeDumper::createAccessSpecifier(AccessSpecifier AS) {
  switch (AS) {
  case AS_none: return "none";
  case AS_private: return "private";
  case AS_protected: return "protected";
  case AS_public: return "public";
  }
  llvm_unreachable("Unknown access specifier");
}

llvm::json::Object
JSONNodeDumper::createCXXBaseSpecifier(const CXXBaseSpecifier &BS) {
  llvm::json::Object Ret;

  Ret["type"] = createQualType(BS.getType());
  Ret["access"] = createAccessSpecifier(BS.getAccessSpecifier());
  Ret["writtenAccess"] =
      createAccessSpecifier(BS.getAccessSpecifierAsWritten());
  if (BS.isVirtual())
    Ret["isVirtual"] = true;
  if (BS.isPackExpansion())
    Ret["isPackExpansion"] = true;

  return Ret;
}

void JSONNodeDumper::VisitTypedefType(const TypedefType *TT) {
  JOS.attribute("decl", createBareDeclRef(TT->getDecl()));
}

void JSONNodeDumper::VisitFunctionType(const FunctionType *T) {
  FunctionType::ExtInfo E = T->getExtInfo();
  attributeOnlyIfTrue("noreturn", E.getNoReturn());
  attributeOnlyIfTrue("producesResult", E.getProducesResult());
  if (E.getHasRegParm())
    JOS.attribute("regParm", E.getRegParm());
  JOS.attribute("cc", FunctionType::getNameForCallConv(E.getCC()));
}

void JSONNodeDumper::VisitFunctionProtoType(const FunctionProtoType *T) {
  FunctionProtoType::ExtProtoInfo E = T->getExtProtoInfo();
  attributeOnlyIfTrue("trailingReturn", E.HasTrailingReturn);
  attributeOnlyIfTrue("const", T->isConst());
  attributeOnlyIfTrue("volatile", T->isVolatile());
  attributeOnlyIfTrue("restrict", T->isRestrict());
  attributeOnlyIfTrue("variadic", E.Variadic);
  switch (E.RefQualifier) {
  case RQ_LValue: JOS.attribute("refQualifier", "&"); break;
  case RQ_RValue: JOS.attribute("refQualifier", "&&"); break;
  case RQ_None: break;
  }
  switch (E.ExceptionSpec.Type) {
  case EST_DynamicNone:
  case EST_Dynamic: {
    JOS.attribute("exceptionSpec", "throw");
    llvm::json::Array Types;
    for (QualType QT : E.ExceptionSpec.Exceptions)
      Types.push_back(createQualType(QT));
    JOS.attribute("exceptionTypes", std::move(Types));
  } break;
  case EST_MSAny:
    JOS.attribute("exceptionSpec", "throw");
    JOS.attribute("throwsAny", true);
    break;
  case EST_BasicNoexcept:
    JOS.attribute("exceptionSpec", "noexcept");
    break;
  case EST_NoexceptTrue:
  case EST_NoexceptFalse:
    JOS.attribute("exceptionSpec", "noexcept");
    JOS.attribute("conditionEvaluatesTo",
                E.ExceptionSpec.Type == EST_NoexceptTrue);
    //JOS.attributeWithCall("exceptionSpecExpr",
    //                    [this, E]() { Visit(E.ExceptionSpec.NoexceptExpr); });
    break;

  // FIXME: I cannot find a way to trigger these cases while dumping the AST. I
  // suspect you can only run into them when executing an AST dump from within
  // the debugger, which is not a use case we worry about for the JSON dumping
  // feature.
  case EST_DependentNoexcept:
  case EST_Unevaluated:
  case EST_Uninstantiated:
  case EST_Unparsed:
  case EST_None: break;
  }
  VisitFunctionType(T);
}

void JSONNodeDumper::VisitNamedDecl(const NamedDecl *ND) {
  if (ND && ND->getDeclName())
    JOS.attribute("name", ND->getNameAsString());
}

void JSONNodeDumper::VisitTypedefDecl(const TypedefDecl *TD) {
  VisitNamedDecl(TD);
  JOS.attribute("type", createQualType(TD->getUnderlyingType()));
}

void JSONNodeDumper::VisitTypeAliasDecl(const TypeAliasDecl *TAD) {
  VisitNamedDecl(TAD);
  JOS.attribute("type", createQualType(TAD->getUnderlyingType()));
}

void JSONNodeDumper::VisitNamespaceDecl(const NamespaceDecl *ND) {
  VisitNamedDecl(ND);
  attributeOnlyIfTrue("isInline", ND->isInline());
  if (!ND->isOriginalNamespace())
    JOS.attribute("originalNamespace",
                  createBareDeclRef(ND->getOriginalNamespace()));
}

void JSONNodeDumper::VisitUsingDirectiveDecl(const UsingDirectiveDecl *UDD) {
  JOS.attribute("nominatedNamespace",
                createBareDeclRef(UDD->getNominatedNamespace()));
}

void JSONNodeDumper::VisitNamespaceAliasDecl(const NamespaceAliasDecl *NAD) {
  VisitNamedDecl(NAD);
  JOS.attribute("aliasedNamespace",
                createBareDeclRef(NAD->getAliasedNamespace()));
}

void JSONNodeDumper::VisitUsingDecl(const UsingDecl *UD) {
  std::string Name;
  if (const NestedNameSpecifier *NNS = UD->getQualifier()) {
    llvm::raw_string_ostream SOS(Name);
    NNS->print(SOS, UD->getASTContext().getPrintingPolicy());
  }
  Name += UD->getNameAsString();
  JOS.attribute("name", Name);
}

void JSONNodeDumper::VisitUsingShadowDecl(const UsingShadowDecl *USD) {
  JOS.attribute("target", createBareDeclRef(USD->getTargetDecl()));
}

void JSONNodeDumper::VisitVarDecl(const VarDecl *VD) {
  VisitNamedDecl(VD);
  JOS.attribute("type", createQualType(VD->getType()));

  StorageClass SC = VD->getStorageClass();
  if (SC != SC_None)
    JOS.attribute("storageClass", VarDecl::getStorageClassSpecifierString(SC));
  switch (VD->getTLSKind()) {
  case VarDecl::TLS_Dynamic: JOS.attribute("tls", "dynamic"); break;
  case VarDecl::TLS_Static: JOS.attribute("tls", "static"); break;
  case VarDecl::TLS_None: break;
  }
  attributeOnlyIfTrue("nrvo", VD->isNRVOVariable());
  attributeOnlyIfTrue("inline", VD->isInline());
  attributeOnlyIfTrue("constexpr", VD->isConstexpr());
  attributeOnlyIfTrue("modulePrivate", VD->isModulePrivate());
  if (VD->hasInit()) {
    switch (VD->getInitStyle()) {
    case VarDecl::CInit: JOS.attribute("init", "c");  break;
    case VarDecl::CallInit: JOS.attribute("init", "call"); break;
    case VarDecl::ListInit: JOS.attribute("init", "list"); break;
    }
  }
  attributeOnlyIfTrue("isParameterPack", VD->isParameterPack());
}

void JSONNodeDumper::VisitFieldDecl(const FieldDecl *FD) {
  VisitNamedDecl(FD);
  JOS.attribute("type", createQualType(FD->getType()));
  attributeOnlyIfTrue("mutable", FD->isMutable());
  attributeOnlyIfTrue("modulePrivate", FD->isModulePrivate());
  attributeOnlyIfTrue("isBitfield", FD->isBitField());
  attributeOnlyIfTrue("hasInClassInitializer", FD->hasInClassInitializer());
}

void JSONNodeDumper::VisitFunctionDecl(const FunctionDecl *FD) {
  VisitNamedDecl(FD);
  JOS.attribute("type", createQualType(FD->getType()));
  StorageClass SC = FD->getStorageClass();
  if (SC != SC_None)
    JOS.attribute("storageClass", VarDecl::getStorageClassSpecifierString(SC));
  attributeOnlyIfTrue("inline", FD->isInlineSpecified());
  attributeOnlyIfTrue("virtual", FD->isVirtualAsWritten());
  attributeOnlyIfTrue("pure", FD->isPure());
  attributeOnlyIfTrue("explicitlyDeleted", FD->isDeletedAsWritten());
  attributeOnlyIfTrue("constexpr", FD->isConstexpr());
  if (FD->isDefaulted())
    JOS.attribute("explicitlyDefaulted",
                  FD->isDeleted() ? "deleted" : "default");
}

void JSONNodeDumper::VisitEnumDecl(const EnumDecl *ED) {
  VisitNamedDecl(ED);
  if (ED->isFixed())
    JOS.attribute("fixedUnderlyingType", createQualType(ED->getIntegerType()));
  if (ED->isScoped())
    JOS.attribute("scopedEnumTag",
                  ED->isScopedUsingClassTag() ? "class" : "struct");
}
void JSONNodeDumper::VisitEnumConstantDecl(const EnumConstantDecl *ECD) {
  VisitNamedDecl(ECD);
  JOS.attribute("type", createQualType(ECD->getType()));
}

void JSONNodeDumper::VisitRecordDecl(const RecordDecl *RD) {
  VisitNamedDecl(RD);
  JOS.attribute("tagUsed", RD->getKindName());
  attributeOnlyIfTrue("completeDefinition", RD->isCompleteDefinition());
}
void JSONNodeDumper::VisitCXXRecordDecl(const CXXRecordDecl *RD) {
  VisitRecordDecl(RD);

  // All other information requires a complete definition.
  if (!RD->isCompleteDefinition())
    return;

  JOS.attribute("definitionData", createCXXRecordDefinitionData(RD));
  if (RD->getNumBases()) {
    JOS.attributeArray("bases", [this, RD] {
      for (const auto &Spec : RD->bases())
        JOS.value(createCXXBaseSpecifier(Spec));
    });
  }
}

void JSONNodeDumper::VisitTemplateTypeParmDecl(const TemplateTypeParmDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("tagUsed", D->wasDeclaredWithTypename() ? "typename" : "class");
  JOS.attribute("depth", D->getDepth());
  JOS.attribute("index", D->getIndex());
  attributeOnlyIfTrue("isParameterPack", D->isParameterPack());
}

void JSONNodeDumper::VisitNonTypeTemplateParmDecl(
    const NonTypeTemplateParmDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("type", createQualType(D->getType()));
  JOS.attribute("depth", D->getDepth());
  JOS.attribute("index", D->getIndex());
  attributeOnlyIfTrue("isParameterPack", D->isParameterPack());
}

void JSONNodeDumper::VisitTemplateTemplateParmDecl(
    const TemplateTemplateParmDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("depth", D->getDepth());
  JOS.attribute("index", D->getIndex());
  attributeOnlyIfTrue("isParameterPack", D->isParameterPack());
}

void JSONNodeDumper::VisitLinkageSpecDecl(const LinkageSpecDecl *LSD) {
  StringRef Lang;
  switch (LSD->getLanguage()) {
  case LinkageSpecDecl::lang_c: Lang = "C"; break;
  case LinkageSpecDecl::lang_cxx: Lang = "C++"; break;
  }
  JOS.attribute("language", Lang);
  attributeOnlyIfTrue("hasBraces", LSD->hasBraces());
}

void JSONNodeDumper::VisitAccessSpecDecl(const AccessSpecDecl *ASD) {
  JOS.attribute("access", createAccessSpecifier(ASD->getAccess()));
}

void JSONNodeDumper::VisitFriendDecl(const FriendDecl *FD) {
  if (const TypeSourceInfo *T = FD->getFriendType())
    JOS.attribute("type", createQualType(T->getType()));
}

void JSONNodeDumper::VisitObjCIvarDecl(const ObjCIvarDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("type", createQualType(D->getType()));
  attributeOnlyIfTrue("synthesized", D->getSynthesize());
  switch (D->getAccessControl()) {
  case ObjCIvarDecl::None: JOS.attribute("access", "none"); break;
  case ObjCIvarDecl::Private: JOS.attribute("access", "private"); break;
  case ObjCIvarDecl::Protected: JOS.attribute("access", "protected"); break;
  case ObjCIvarDecl::Public: JOS.attribute("access", "public"); break;
  case ObjCIvarDecl::Package: JOS.attribute("access", "package"); break;
  }
}

void JSONNodeDumper::VisitObjCMethodDecl(const ObjCMethodDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("returnType", createQualType(D->getReturnType()));
  JOS.attribute("instance", D->isInstanceMethod());
  attributeOnlyIfTrue("variadic", D->isVariadic());
}

void JSONNodeDumper::VisitObjCTypeParamDecl(const ObjCTypeParamDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("type", createQualType(D->getUnderlyingType()));
  attributeOnlyIfTrue("bounded", D->hasExplicitBound());
  switch (D->getVariance()) {
  case ObjCTypeParamVariance::Invariant:
    break;
  case ObjCTypeParamVariance::Covariant:
    JOS.attribute("variance", "covariant");
    break;
  case ObjCTypeParamVariance::Contravariant:
    JOS.attribute("variance", "contravariant");
    break;
  }
}

void JSONNodeDumper::VisitObjCCategoryDecl(const ObjCCategoryDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("interface", createBareDeclRef(D->getClassInterface()));
  JOS.attribute("implementation", createBareDeclRef(D->getImplementation()));

  llvm::json::Array Protocols;
  for (const auto* P : D->protocols())
    Protocols.push_back(createBareDeclRef(P));
  if (!Protocols.empty())
    JOS.attribute("protocols", std::move(Protocols));
}

void JSONNodeDumper::VisitObjCCategoryImplDecl(const ObjCCategoryImplDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("interface", createBareDeclRef(D->getClassInterface()));
  JOS.attribute("categoryDecl", createBareDeclRef(D->getCategoryDecl()));
}

void JSONNodeDumper::VisitObjCProtocolDecl(const ObjCProtocolDecl *D) {
  VisitNamedDecl(D);

  llvm::json::Array Protocols;
  for (const auto *P : D->protocols())
    Protocols.push_back(createBareDeclRef(P));
  if (!Protocols.empty())
    JOS.attribute("protocols", std::move(Protocols));
}

void JSONNodeDumper::VisitObjCInterfaceDecl(const ObjCInterfaceDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("super", createBareDeclRef(D->getSuperClass()));
  JOS.attribute("implementation", createBareDeclRef(D->getImplementation()));

  llvm::json::Array Protocols;
  for (const auto* P : D->protocols())
    Protocols.push_back(createBareDeclRef(P));
  if (!Protocols.empty())
    JOS.attribute("protocols", std::move(Protocols));
}

void JSONNodeDumper::VisitObjCImplementationDecl(
    const ObjCImplementationDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("super", createBareDeclRef(D->getSuperClass()));
  JOS.attribute("interface", createBareDeclRef(D->getClassInterface()));
}

void JSONNodeDumper::VisitObjCCompatibleAliasDecl(
    const ObjCCompatibleAliasDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("interface", createBareDeclRef(D->getClassInterface()));
}

void JSONNodeDumper::VisitObjCPropertyDecl(const ObjCPropertyDecl *D) {
  VisitNamedDecl(D);
  JOS.attribute("type", createQualType(D->getType()));

  switch (D->getPropertyImplementation()) {
  case ObjCPropertyDecl::None: break;
  case ObjCPropertyDecl::Required: JOS.attribute("control", "required"); break;
  case ObjCPropertyDecl::Optional: JOS.attribute("control", "optional"); break;
  }
  
  ObjCPropertyDecl::PropertyAttributeKind Attrs = D->getPropertyAttributes();
  if (Attrs != ObjCPropertyDecl::OBJC_PR_noattr) {
    if (Attrs & ObjCPropertyDecl::OBJC_PR_getter)
      JOS.attribute("getter", createBareDeclRef(D->getGetterMethodDecl()));
    if (Attrs & ObjCPropertyDecl::OBJC_PR_setter)
      JOS.attribute("setter", createBareDeclRef(D->getSetterMethodDecl()));
    attributeOnlyIfTrue("readonly", Attrs & ObjCPropertyDecl::OBJC_PR_readonly);
    attributeOnlyIfTrue("assign", Attrs & ObjCPropertyDecl::OBJC_PR_assign);
    attributeOnlyIfTrue("readwrite",
                        Attrs & ObjCPropertyDecl::OBJC_PR_readwrite);
    attributeOnlyIfTrue("retain", Attrs & ObjCPropertyDecl::OBJC_PR_retain);
    attributeOnlyIfTrue("copy", Attrs & ObjCPropertyDecl::OBJC_PR_copy);
    attributeOnlyIfTrue("nonatomic",
                        Attrs & ObjCPropertyDecl::OBJC_PR_nonatomic);
    attributeOnlyIfTrue("atomic", Attrs & ObjCPropertyDecl::OBJC_PR_atomic);
    attributeOnlyIfTrue("weak", Attrs & ObjCPropertyDecl::OBJC_PR_weak);
    attributeOnlyIfTrue("strong", Attrs & ObjCPropertyDecl::OBJC_PR_strong);
    attributeOnlyIfTrue("unsafe_unretained",
                        Attrs & ObjCPropertyDecl::OBJC_PR_unsafe_unretained);
    attributeOnlyIfTrue("class", Attrs & ObjCPropertyDecl::OBJC_PR_class);
    attributeOnlyIfTrue("nullability",
                        Attrs & ObjCPropertyDecl::OBJC_PR_nullability);
    attributeOnlyIfTrue("null_resettable",
                        Attrs & ObjCPropertyDecl::OBJC_PR_null_resettable);
  }
}

void JSONNodeDumper::VisitObjCPropertyImplDecl(const ObjCPropertyImplDecl *D) {
  VisitNamedDecl(D->getPropertyDecl());
  JOS.attribute("implKind", D->getPropertyImplementation() ==
                                    ObjCPropertyImplDecl::Synthesize
                                ? "synthesize"
                                : "dynamic");
  JOS.attribute("propertyDecl", createBareDeclRef(D->getPropertyDecl()));
  JOS.attribute("ivarDecl", createBareDeclRef(D->getPropertyIvarDecl()));
}

void JSONNodeDumper::VisitBlockDecl(const BlockDecl *D) {
  attributeOnlyIfTrue("variadic", D->isVariadic());
  attributeOnlyIfTrue("capturesThis", D->capturesCXXThis());
}

void JSONNodeDumper::VisitDeclRefExpr(const DeclRefExpr *DRE) {
  JOS.attribute("referencedDecl", createBareDeclRef(DRE->getDecl()));
  if (DRE->getDecl() != DRE->getFoundDecl())
    JOS.attribute("foundReferencedDecl",
                  createBareDeclRef(DRE->getFoundDecl()));
}

void JSONNodeDumper::VisitPredefinedExpr(const PredefinedExpr *PE) {
  JOS.attribute("name", PredefinedExpr::getIdentKindName(PE->getIdentKind()));
}

void JSONNodeDumper::VisitUnaryOperator(const UnaryOperator *UO) {
  JOS.attribute("isPostfix", UO->isPostfix());
  JOS.attribute("opcode", UnaryOperator::getOpcodeStr(UO->getOpcode()));
  if (!UO->canOverflow())
    JOS.attribute("canOverflow", false);
}

void JSONNodeDumper::VisitBinaryOperator(const BinaryOperator *BO) {
  JOS.attribute("opcode", BinaryOperator::getOpcodeStr(BO->getOpcode()));
}

void JSONNodeDumper::VisitCompoundAssignOperator(
    const CompoundAssignOperator *CAO) {
  VisitBinaryOperator(CAO);
  JOS.attribute("computeLHSType", createQualType(CAO->getComputationLHSType()));
  JOS.attribute("computeResultType",
                createQualType(CAO->getComputationResultType()));
}

void JSONNodeDumper::VisitMemberExpr(const MemberExpr *ME) {
  // Note, we always write this Boolean field because the information it conveys
  // is critical to understanding the AST node.
  JOS.attribute("isArrow", ME->isArrow());
  JOS.attribute("referencedMemberDecl",
                createPointerRepresentation(ME->getMemberDecl()));
}

void JSONNodeDumper::VisitCXXNewExpr(const CXXNewExpr *NE) {
  attributeOnlyIfTrue("isGlobal", NE->isGlobalNew());
  attributeOnlyIfTrue("isArray", NE->isArray());
  attributeOnlyIfTrue("isPlacement", NE->getNumPlacementArgs() != 0);
  switch (NE->getInitializationStyle()) {
  case CXXNewExpr::NoInit: break;
  case CXXNewExpr::CallInit: JOS.attribute("initStyle", "call"); break;
  case CXXNewExpr::ListInit: JOS.attribute("initStyle", "list"); break;
  }
  if (const FunctionDecl *FD = NE->getOperatorNew())
    JOS.attribute("operatorNewDecl", createBareDeclRef(FD));
  if (const FunctionDecl *FD = NE->getOperatorDelete())
    JOS.attribute("operatorDeleteDecl", createBareDeclRef(FD));
}
void JSONNodeDumper::VisitCXXDeleteExpr(const CXXDeleteExpr *DE) {
  attributeOnlyIfTrue("isGlobal", DE->isGlobalDelete());
  attributeOnlyIfTrue("isArray", DE->isArrayForm());
  attributeOnlyIfTrue("isArrayAsWritten", DE->isArrayFormAsWritten());
  if (const FunctionDecl *FD = DE->getOperatorDelete())
    JOS.attribute("operatorDeleteDecl", createBareDeclRef(FD));
}

void JSONNodeDumper::VisitCXXThisExpr(const CXXThisExpr *TE) {
  attributeOnlyIfTrue("implicit", TE->isImplicit());
}

void JSONNodeDumper::VisitCastExpr(const CastExpr *CE) {
  JOS.attribute("castKind", CE->getCastKindName());
  llvm::json::Array Path = createCastPath(CE);
  if (!Path.empty())
    JOS.attribute("path", std::move(Path));
  // FIXME: This may not be useful information as it can be obtusely gleaned
  // from the inner[] array.
  if (const NamedDecl *ND = CE->getConversionFunction())
    JOS.attribute("conversionFunc", createBareDeclRef(ND));
}

void JSONNodeDumper::VisitImplicitCastExpr(const ImplicitCastExpr *ICE) {
  VisitCastExpr(ICE);
  attributeOnlyIfTrue("isPartOfExplicitCast", ICE->isPartOfExplicitCast());
}

void JSONNodeDumper::VisitCallExpr(const CallExpr *CE) {
  attributeOnlyIfTrue("adl", CE->usesADL());
}

void JSONNodeDumper::VisitUnaryExprOrTypeTraitExpr(
    const UnaryExprOrTypeTraitExpr *TTE) {
  switch (TTE->getKind()) {
  case UETT_SizeOf: JOS.attribute("name", "sizeof"); break;
  case UETT_AlignOf: JOS.attribute("name", "alignof"); break;
  case UETT_VecStep:  JOS.attribute("name", "vec_step"); break;
  case UETT_PreferredAlignOf:  JOS.attribute("name", "__alignof"); break;
  case UETT_OpenMPRequiredSimdAlign:
    JOS.attribute("name", "__builtin_omp_required_simd_align"); break;
  }
  if (TTE->isArgumentType())
    JOS.attribute("argType", createQualType(TTE->getArgumentType()));
}

void JSONNodeDumper::VisitUnresolvedLookupExpr(
    const UnresolvedLookupExpr *ULE) {
  JOS.attribute("usesADL", ULE->requiresADL());
  JOS.attribute("name", ULE->getName().getAsString());

  JOS.attributeArray("lookups", [this, ULE] {
    for (const NamedDecl *D : ULE->decls())
      JOS.value(createBareDeclRef(D));
  });
}

void JSONNodeDumper::VisitAddrLabelExpr(const AddrLabelExpr *ALE) {
  JOS.attribute("name", ALE->getLabel()->getName());
  JOS.attribute("labelDeclId", createPointerRepresentation(ALE->getLabel()));
}

void JSONNodeDumper::VisitIntegerLiteral(const IntegerLiteral *IL) {
  JOS.attribute("value",
                IL->getValue().toString(
                    /*Radix=*/10, IL->getType()->isSignedIntegerType()));
}
void JSONNodeDumper::VisitCharacterLiteral(const CharacterLiteral *CL) {
  // FIXME: This should probably print the character literal as a string,
  // rather than as a numerical value. It would be nice if the behavior matched
  // what we do to print a string literal; right now, it is impossible to tell
  // the difference between 'a' and L'a' in C from the JSON output.
  JOS.attribute("value", CL->getValue());
}
void JSONNodeDumper::VisitFixedPointLiteral(const FixedPointLiteral *FPL) {
  JOS.attribute("value", FPL->getValueAsString(/*Radix=*/10));
}
void JSONNodeDumper::VisitFloatingLiteral(const FloatingLiteral *FL) {
  JOS.attribute("value", FL->getValueAsApproximateDouble());
}
void JSONNodeDumper::VisitStringLiteral(const StringLiteral *SL) {
  std::string Buffer;
  llvm::raw_string_ostream SS(Buffer);
  SL->outputString(SS);
  JOS.attribute("value", SS.str());
}
void JSONNodeDumper::VisitCXXBoolLiteralExpr(const CXXBoolLiteralExpr *BLE) {
  JOS.attribute("value", BLE->getValue());
}

void JSONNodeDumper::VisitIfStmt(const IfStmt *IS) {
  attributeOnlyIfTrue("hasInit", IS->hasInitStorage());
  attributeOnlyIfTrue("hasVar", IS->hasVarStorage());
  attributeOnlyIfTrue("hasElse", IS->hasElseStorage());
  attributeOnlyIfTrue("isConstexpr", IS->isConstexpr());
}

void JSONNodeDumper::VisitSwitchStmt(const SwitchStmt *SS) {
  attributeOnlyIfTrue("hasInit", SS->hasInitStorage());
  attributeOnlyIfTrue("hasVar", SS->hasVarStorage());
}
void JSONNodeDumper::VisitCaseStmt(const CaseStmt *CS) {
  attributeOnlyIfTrue("isGNURange", CS->caseStmtIsGNURange());
}

void JSONNodeDumper::VisitLabelStmt(const LabelStmt *LS) {
  JOS.attribute("name", LS->getName());
  JOS.attribute("declId", createPointerRepresentation(LS->getDecl()));
}
void JSONNodeDumper::VisitGotoStmt(const GotoStmt *GS) {
  JOS.attribute("targetLabelDeclId",
                createPointerRepresentation(GS->getLabel()));
}

void JSONNodeDumper::VisitWhileStmt(const WhileStmt *WS) {
  attributeOnlyIfTrue("hasVar", WS->hasVarStorage());
}

StringRef JSONNodeDumper::getCommentCommandName(unsigned CommandID) const {
  if (Traits)
    return Traits->getCommandInfo(CommandID)->Name;
  if (const comments::CommandInfo *Info =
          comments::CommandTraits::getBuiltinCommandInfo(CommandID))
    return Info->Name;
  return "<invalid>";
}

void JSONNodeDumper::visitTextComment(const comments::TextComment *C,
                                      const comments::FullComment *) {
  JOS.attribute("text", C->getText());
}

void JSONNodeDumper::visitInlineCommandComment(
    const comments::InlineCommandComment *C, const comments::FullComment *) {
  JOS.attribute("name", getCommentCommandName(C->getCommandID()));

  switch (C->getRenderKind()) {
  case comments::InlineCommandComment::RenderNormal:
    JOS.attribute("renderKind", "normal");
    break;
  case comments::InlineCommandComment::RenderBold:
    JOS.attribute("renderKind", "bold");
    break;
  case comments::InlineCommandComment::RenderEmphasized:
    JOS.attribute("renderKind", "emphasized");
    break;
  case comments::InlineCommandComment::RenderMonospaced:
    JOS.attribute("renderKind", "monospaced");
    break;
  }

  llvm::json::Array Args;
  for (unsigned I = 0, E = C->getNumArgs(); I < E; ++I)
    Args.push_back(C->getArgText(I));

  if (!Args.empty())
    JOS.attribute("args", std::move(Args));
}

void JSONNodeDumper::visitHTMLStartTagComment(
    const comments::HTMLStartTagComment *C, const comments::FullComment *) {
  JOS.attribute("name", C->getTagName());
  attributeOnlyIfTrue("selfClosing", C->isSelfClosing());
  attributeOnlyIfTrue("malformed", C->isMalformed());

  llvm::json::Array Attrs;
  for (unsigned I = 0, E = C->getNumAttrs(); I < E; ++I)
    Attrs.push_back(
        {{"name", C->getAttr(I).Name}, {"value", C->getAttr(I).Value}});

  if (!Attrs.empty())
    JOS.attribute("attrs", std::move(Attrs));
}

void JSONNodeDumper::visitHTMLEndTagComment(
    const comments::HTMLEndTagComment *C, const comments::FullComment *) {
  JOS.attribute("name", C->getTagName());
}

void JSONNodeDumper::visitBlockCommandComment(
    const comments::BlockCommandComment *C, const comments::FullComment *) {
  JOS.attribute("name", getCommentCommandName(C->getCommandID()));

  llvm::json::Array Args;
  for (unsigned I = 0, E = C->getNumArgs(); I < E; ++I)
    Args.push_back(C->getArgText(I));

  if (!Args.empty())
    JOS.attribute("args", std::move(Args));
}

void JSONNodeDumper::visitParamCommandComment(
    const comments::ParamCommandComment *C, const comments::FullComment *FC) {
  switch (C->getDirection()) {
  case comments::ParamCommandComment::In:
    JOS.attribute("direction", "in");
    break;
  case comments::ParamCommandComment::Out:
    JOS.attribute("direction", "out");
    break;
  case comments::ParamCommandComment::InOut:
    JOS.attribute("direction", "in,out");
    break;
  }
  attributeOnlyIfTrue("explicit", C->isDirectionExplicit());

  if (C->hasParamName())
    JOS.attribute("param", C->isParamIndexValid() ? C->getParamName(FC)
                                                  : C->getParamNameAsWritten());

  if (C->isParamIndexValid() && !C->isVarArgParam())
    JOS.attribute("paramIdx", C->getParamIndex());
}

void JSONNodeDumper::visitTParamCommandComment(
    const comments::TParamCommandComment *C, const comments::FullComment *FC) {
  if (C->hasParamName())
    JOS.attribute("param", C->isPositionValid() ? C->getParamName(FC)
                                                : C->getParamNameAsWritten());
  if (C->isPositionValid()) {
    llvm::json::Array Positions;
    for (unsigned I = 0, E = C->getDepth(); I < E; ++I)
      Positions.push_back(C->getIndex(I));

    if (!Positions.empty())
      JOS.attribute("positions", std::move(Positions));
  }
}

void JSONNodeDumper::visitVerbatimBlockComment(
    const comments::VerbatimBlockComment *C, const comments::FullComment *) {
  JOS.attribute("name", getCommentCommandName(C->getCommandID()));
  JOS.attribute("closeName", C->getCloseName());
}

void JSONNodeDumper::visitVerbatimBlockLineComment(
    const comments::VerbatimBlockLineComment *C,
    const comments::FullComment *) {
  JOS.attribute("text", C->getText());
}

void JSONNodeDumper::visitVerbatimLineComment(
    const comments::VerbatimLineComment *C, const comments::FullComment *) {
  JOS.attribute("text", C->getText());
}
