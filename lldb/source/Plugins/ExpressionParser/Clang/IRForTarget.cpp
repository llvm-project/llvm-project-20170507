//===-- IRForTarget.cpp -----------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IRForTarget.h"

#include "ClangExpressionDeclMap.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO.h"

#include "clang/AST/ASTContext.h"

#include "lldb/Core/dwarf.h"
#include "lldb/Expression/IRExecutionUnit.h"
#include "lldb/Expression/IRInterpreter.h"
#include "lldb/Symbol/ClangASTContext.h"
#include "lldb/Symbol/ClangUtil.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/DataBufferHeap.h"
#include "lldb/Utility/Endian.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/Scalar.h"
#include "lldb/Utility/StreamString.h"

#include <map>

using namespace llvm;

static char ID;

typedef SmallVector<Instruction *, 2> InstrList;

IRForTarget::FunctionValueCache::FunctionValueCache(Maker const &maker)
    : m_maker(maker), m_values() {}

IRForTarget::FunctionValueCache::~FunctionValueCache() {}

llvm::Value *
IRForTarget::FunctionValueCache::GetValue(llvm::Function *function) {
  if (!m_values.count(function)) {
    llvm::Value *ret = m_maker(function);
    m_values[function] = ret;
    return ret;
  }
  return m_values[function];
}

static llvm::Value *FindEntryInstruction(llvm::Function *function) {
  if (function->empty())
    return nullptr;

  return function->getEntryBlock().getFirstNonPHIOrDbg();
}

IRForTarget::IRForTarget(lldb_private::ClangExpressionDeclMap *decl_map,
                         bool resolve_vars,
                         lldb_private::IRExecutionUnit &execution_unit,
                         lldb_private::Stream &error_stream,
                         const char *func_name)
    : ModulePass(ID), m_resolve_vars(resolve_vars), m_func_name(func_name),
      m_module(nullptr), m_decl_map(decl_map),
      m_CFStringCreateWithBytes(nullptr), m_sel_registerName(nullptr),
      m_objc_getClass(nullptr), m_intptr_ty(nullptr),
      m_error_stream(error_stream), m_execution_unit(execution_unit),
      m_result_store(nullptr), m_result_is_pointer(false),
      m_reloc_placeholder(nullptr),
      m_entry_instruction_finder(FindEntryInstruction) {}

/* Handy utility functions used at several places in the code */

static std::string PrintValue(const Value *value, bool truncate = false) {
  std::string s;
  if (value) {
    raw_string_ostream rso(s);
    value->print(rso);
    rso.flush();
    if (truncate)
      s.resize(s.length() - 1);
  }
  return s;
}

static std::string PrintType(const llvm::Type *type, bool truncate = false) {
  std::string s;
  raw_string_ostream rso(s);
  type->print(rso);
  rso.flush();
  if (truncate)
    s.resize(s.length() - 1);
  return s;
}

IRForTarget::~IRForTarget() {}

bool IRForTarget::FixFunctionLinkage(llvm::Function &llvm_function) {
  llvm_function.setLinkage(GlobalValue::ExternalLinkage);

  return true;
}

clang::NamedDecl *IRForTarget::DeclForGlobal(const GlobalValue *global_val,
                                             Module *module) {
  NamedMDNode *named_metadata =
      module->getNamedMetadata("clang.global.decl.ptrs");

  if (!named_metadata)
    return nullptr;

  unsigned num_nodes = named_metadata->getNumOperands();
  unsigned node_index;

  for (node_index = 0; node_index < num_nodes; ++node_index) {
    llvm::MDNode *metadata_node =
        dyn_cast<llvm::MDNode>(named_metadata->getOperand(node_index));
    if (!metadata_node)
      return nullptr;

    if (metadata_node->getNumOperands() != 2)
      continue;

    if (mdconst::dyn_extract_or_null<GlobalValue>(
            metadata_node->getOperand(0)) != global_val)
      continue;

    ConstantInt *constant_int =
        mdconst::dyn_extract<ConstantInt>(metadata_node->getOperand(1));

    if (!constant_int)
      return nullptr;

    uintptr_t ptr = constant_int->getZExtValue();

    return reinterpret_cast<clang::NamedDecl *>(ptr);
  }

  return nullptr;
}

clang::NamedDecl *IRForTarget::DeclForGlobal(GlobalValue *global_val) {
  return DeclForGlobal(global_val, m_module);
}

/// Returns true iff the mangled symbol is for a static guard variable.
static bool isGuardVariableSymbol(llvm::StringRef mangled_symbol) {
  return mangled_symbol.startswith("_ZGV") || // Itanium ABI guard variable
         mangled_symbol.startswith("@4IA");   // Microsoft ABI guard variable
}

bool IRForTarget::CreateResultVariable(llvm::Function &llvm_function) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  if (!m_resolve_vars)
    return true;

  // Find the result variable.  If it doesn't exist, we can give up right here.

  ValueSymbolTable &value_symbol_table = m_module->getValueSymbolTable();

  llvm::StringRef result_name;
  bool found_result = false;

  for (StringMapEntry<llvm::Value *> &value_symbol : value_symbol_table) {
    result_name = value_symbol.first();

    if (result_name.contains("$__lldb_expr_result_ptr") &&
        !isGuardVariableSymbol(result_name)) {
      found_result = true;
      m_result_is_pointer = true;
      break;
    }

    if (result_name.contains("$__lldb_expr_result") &&
        !isGuardVariableSymbol(result_name)) {
      found_result = true;
      m_result_is_pointer = false;
      break;
    }
  }

  if (!found_result) {
    LLDB_LOG(log, "Couldn't find result variable");

    return true;
  }

  LLDB_LOG(log, "Result name: \"{0}\"", result_name);

  Value *result_value = m_module->getNamedValue(result_name);

  if (!result_value) {
    LLDB_LOG(log, "Result variable had no data");

    m_error_stream.Format("Internal error [IRForTarget]: Result variable's "
                          "name ({0}) exists, but not its definition\n",
                          result_name);

    return false;
  }

  LLDB_LOG(log, "Found result in the IR: \"{0}\"",
           PrintValue(result_value, false));

  GlobalVariable *result_global = dyn_cast<GlobalVariable>(result_value);

  if (!result_global) {
    LLDB_LOG(log, "Result variable isn't a GlobalVariable");

    m_error_stream.Format("Internal error [IRForTarget]: Result variable ({0}) "
                          "is defined, but is not a global variable\n",
                          result_name);

    return false;
  }

  clang::NamedDecl *result_decl = DeclForGlobal(result_global);
  if (!result_decl) {
    LLDB_LOG(log, "Result variable doesn't have a corresponding Decl");

    m_error_stream.Format("Internal error [IRForTarget]: Result variable ({0}) "
                          "does not have a corresponding Clang entity\n",
                          result_name);

    return false;
  }

  if (log) {
    std::string decl_desc_str;
    raw_string_ostream decl_desc_stream(decl_desc_str);
    result_decl->print(decl_desc_stream);
    decl_desc_stream.flush();

    LLDB_LOG(log, "Found result decl: \"{0}\"", decl_desc_str);
  }

  clang::VarDecl *result_var = dyn_cast<clang::VarDecl>(result_decl);
  if (!result_var) {
    LLDB_LOG(log, "Result variable Decl isn't a VarDecl");

    m_error_stream.Format("Internal error [IRForTarget]: Result variable "
                          "({0})'s corresponding Clang entity isn't a "
                          "variable\n",
                          result_name);

    return false;
  }

  // Get the next available result name from m_decl_map and create the
  // persistent variable for it

  // If the result is an Lvalue, it is emitted as a pointer; see
  // ASTResultSynthesizer::SynthesizeBodyResult.
  if (m_result_is_pointer) {
    clang::QualType pointer_qual_type = result_var->getType();
    const clang::Type *pointer_type = pointer_qual_type.getTypePtr();

    const clang::PointerType *pointer_pointertype =
        pointer_type->getAs<clang::PointerType>();
    const clang::ObjCObjectPointerType *pointer_objcobjpointertype =
        pointer_type->getAs<clang::ObjCObjectPointerType>();

    if (pointer_pointertype) {
      clang::QualType element_qual_type = pointer_pointertype->getPointeeType();

      m_result_type = lldb_private::TypeFromParser(
          element_qual_type.getAsOpaquePtr(),
          lldb_private::ClangASTContext::GetASTContext(
              &result_decl->getASTContext()));
    } else if (pointer_objcobjpointertype) {
      clang::QualType element_qual_type =
          clang::QualType(pointer_objcobjpointertype->getObjectType(), 0);

      m_result_type = lldb_private::TypeFromParser(
          element_qual_type.getAsOpaquePtr(),
          lldb_private::ClangASTContext::GetASTContext(
              &result_decl->getASTContext()));
    } else {
      LLDB_LOG(log, "Expected result to have pointer type, but it did not");

      m_error_stream.Format("Internal error [IRForTarget]: Lvalue result ({0}) "
                            "is not a pointer variable\n",
                            result_name);

      return false;
    }
  } else {
    m_result_type = lldb_private::TypeFromParser(
        result_var->getType().getAsOpaquePtr(),
        lldb_private::ClangASTContext::GetASTContext(
            &result_decl->getASTContext()));
  }

  lldb::TargetSP target_sp(m_execution_unit.GetTarget());
  lldb_private::ExecutionContext exe_ctx(target_sp, true);
  llvm::Optional<uint64_t> bit_size =
      m_result_type.GetBitSize(exe_ctx.GetBestExecutionContextScope());
  if (!bit_size) {
    lldb_private::StreamString type_desc_stream;
    m_result_type.DumpTypeDescription(&type_desc_stream);

    LLDB_LOG(log, "Result type has unknown size");

    m_error_stream.Printf("Error [IRForTarget]: Size of result type '%s' "
                          "couldn't be determined\n",
                          type_desc_stream.GetData());
    return false;
  }

  if (log) {
    lldb_private::StreamString type_desc_stream;
    m_result_type.DumpTypeDescription(&type_desc_stream);

    LLDB_LOG(log, "Result decl type: \"{0}\"", type_desc_stream.GetData());
  }

  m_result_name = lldb_private::ConstString("$RESULT_NAME");

  LLDB_LOG(log, "Creating a new result global: \"{0}\" with size {1}",
           m_result_name, m_result_type.GetByteSize(nullptr).getValueOr(0));

  // Construct a new result global and set up its metadata

  GlobalVariable *new_result_global = new GlobalVariable(
      (*m_module), result_global->getType()->getElementType(),
      false,                                 /* not constant */
      GlobalValue::ExternalLinkage, nullptr, /* no initializer */
      m_result_name.GetCString());

  // It's too late in compilation to create a new VarDecl for this, but we
  // don't need to.  We point the metadata at the old VarDecl.  This creates an
  // odd anomaly: a variable with a Value whose name is something like $0 and a
  // Decl whose name is $__lldb_expr_result.  This condition is handled in
  // ClangExpressionDeclMap::DoMaterialize, and the name of the variable is
  // fixed up.

  ConstantInt *new_constant_int =
      ConstantInt::get(llvm::Type::getInt64Ty(m_module->getContext()),
                       reinterpret_cast<uint64_t>(result_decl), false);

  llvm::Metadata *values[2];
  values[0] = ConstantAsMetadata::get(new_result_global);
  values[1] = ConstantAsMetadata::get(new_constant_int);

  ArrayRef<Metadata *> value_ref(values, 2);

  MDNode *persistent_global_md = MDNode::get(m_module->getContext(), value_ref);
  NamedMDNode *named_metadata =
      m_module->getNamedMetadata("clang.global.decl.ptrs");
  named_metadata->addOperand(persistent_global_md);

  LLDB_LOG(log, "Replacing \"{0}\" with \"{1}\"", PrintValue(result_global),
           PrintValue(new_result_global));

  if (result_global->use_empty()) {
    // We need to synthesize a store for this variable, because otherwise
    // there's nothing to put into its equivalent persistent variable.

    BasicBlock &entry_block(llvm_function.getEntryBlock());
    Instruction *first_entry_instruction(entry_block.getFirstNonPHIOrDbg());

    if (!first_entry_instruction)
      return false;

    if (!result_global->hasInitializer()) {
      LLDB_LOG(log, "Couldn't find initializer for unused variable");

      m_error_stream.Format("Internal error [IRForTarget]: Result variable "
                            "({0}) has no writes and no initializer\n",
                            result_name);

      return false;
    }

    Constant *initializer = result_global->getInitializer();

    StoreInst *synthesized_store =
        new StoreInst(initializer, new_result_global, first_entry_instruction);

    LLDB_LOG(log, "Synthesized result store \"{0}\"\n",
             PrintValue(synthesized_store));
  } else {
    result_global->replaceAllUsesWith(new_result_global);
  }

  if (!m_decl_map->AddPersistentVariable(
          result_decl, m_result_name, m_result_type, true, m_result_is_pointer))
    return false;

  result_global->eraseFromParent();

  return true;
}

bool IRForTarget::RewriteObjCConstString(llvm::GlobalVariable *ns_str,
                                         llvm::GlobalVariable *cstr) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  Type *ns_str_ty = ns_str->getType();

  Type *i8_ptr_ty = Type::getInt8PtrTy(m_module->getContext());
  Type *i32_ty = Type::getInt32Ty(m_module->getContext());
  Type *i8_ty = Type::getInt8Ty(m_module->getContext());

  if (!m_CFStringCreateWithBytes) {
    lldb::addr_t CFStringCreateWithBytes_addr;

    static lldb_private::ConstString g_CFStringCreateWithBytes_str(
        "CFStringCreateWithBytes");

    bool missing_weak = false;
    CFStringCreateWithBytes_addr =
        m_execution_unit.FindSymbol(g_CFStringCreateWithBytes_str, 
                                    missing_weak);
    if (CFStringCreateWithBytes_addr == LLDB_INVALID_ADDRESS || missing_weak) {
        log->PutCString("Couldn't find CFStringCreateWithBytes in the target");

      m_error_stream.Printf("Error [IRForTarget]: Rewriting an Objective-C "
                            "constant string requires "
                            "CFStringCreateWithBytes\n");

      return false;
    }

    LLDB_LOG(log, "Found CFStringCreateWithBytes at {0}",
             CFStringCreateWithBytes_addr);

    // Build the function type:
    //
    // CFStringRef CFStringCreateWithBytes (
    //   CFAllocatorRef alloc,
    //   const UInt8 *bytes,
    //   CFIndex numBytes,
    //   CFStringEncoding encoding,
    //   Boolean isExternalRepresentation
    // );
    //
    // We make the following substitutions:
    //
    // CFStringRef -> i8*
    // CFAllocatorRef -> i8*
    // UInt8 * -> i8*
    // CFIndex -> long (i32 or i64, as appropriate; we ask the module for its
    // pointer size for now) CFStringEncoding -> i32 Boolean -> i8

    Type *arg_type_array[5];

    arg_type_array[0] = i8_ptr_ty;
    arg_type_array[1] = i8_ptr_ty;
    arg_type_array[2] = m_intptr_ty;
    arg_type_array[3] = i32_ty;
    arg_type_array[4] = i8_ty;

    ArrayRef<Type *> CFSCWB_arg_types(arg_type_array, 5);

    llvm::FunctionType *CFSCWB_ty =
        FunctionType::get(ns_str_ty, CFSCWB_arg_types, false);

    // Build the constant containing the pointer to the function
    PointerType *CFSCWB_ptr_ty = PointerType::getUnqual(CFSCWB_ty);
    Constant *CFSCWB_addr_int =
        ConstantInt::get(m_intptr_ty, CFStringCreateWithBytes_addr, false);
    m_CFStringCreateWithBytes = {
        CFSCWB_ty, ConstantExpr::getIntToPtr(CFSCWB_addr_int, CFSCWB_ptr_ty)};
  }

  ConstantDataSequential *string_array = nullptr;

  if (cstr)
    string_array = dyn_cast<ConstantDataSequential>(cstr->getInitializer());

  Constant *alloc_arg = Constant::getNullValue(i8_ptr_ty);
  Constant *bytes_arg = cstr ? ConstantExpr::getBitCast(cstr, i8_ptr_ty)
                             : Constant::getNullValue(i8_ptr_ty);
  Constant *numBytes_arg = ConstantInt::get(
      m_intptr_ty, cstr ? (string_array->getNumElements() - 1) * string_array->getElementByteSize() : 0, false);
 int encoding_flags = 0;
 switch (cstr ? string_array->getElementByteSize() : 1) {
 case 1:
   encoding_flags = 0x08000100; /* 0x08000100 is kCFStringEncodingUTF8 */
   break;
 case 2:
   encoding_flags = 0x0100; /* 0x0100 is kCFStringEncodingUTF16 */
   break;
 case 4:
   encoding_flags = 0x0c000100; /* 0x0c000100 is kCFStringEncodingUTF32 */
   break;
 default:
   encoding_flags = 0x0600; /* fall back to 0x0600, kCFStringEncodingASCII */
   LLDB_LOG(log, "Encountered an Objective-C constant string with unusual "
                 "element size {0}",
            string_array->getElementByteSize());
 }
 Constant *encoding_arg = ConstantInt::get(i32_ty, encoding_flags, false);
 Constant *isExternal_arg =
     ConstantInt::get(i8_ty, 0x0, false); /* 0x0 is false */

 Value *argument_array[5];

 argument_array[0] = alloc_arg;
 argument_array[1] = bytes_arg;
 argument_array[2] = numBytes_arg;
 argument_array[3] = encoding_arg;
 argument_array[4] = isExternal_arg;

 ArrayRef<Value *> CFSCWB_arguments(argument_array, 5);

 FunctionValueCache CFSCWB_Caller(
     [this, &CFSCWB_arguments](llvm::Function *function) -> llvm::Value * {
       return CallInst::Create(
           m_CFStringCreateWithBytes, CFSCWB_arguments,
           "CFStringCreateWithBytes",
           llvm::cast<Instruction>(
               m_entry_instruction_finder.GetValue(function)));
     });

 if (!UnfoldConstant(ns_str, nullptr, CFSCWB_Caller, m_entry_instruction_finder,
                     m_error_stream)) {
   LLDB_LOG(log, "Couldn't replace the NSString with the result of the call");

   m_error_stream.Printf("error [IRForTarget internal]: Couldn't replace an "
                         "Objective-C constant string with a dynamic "
                         "string\n");

   return false;
  }

  ns_str->eraseFromParent();

  return true;
}

bool IRForTarget::RewriteObjCConstStrings() {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  ValueSymbolTable &value_symbol_table = m_module->getValueSymbolTable();

  for (StringMapEntry<llvm::Value *> &value_symbol : value_symbol_table) {
    llvm::StringRef value_name = value_symbol.first();

    if (value_name.contains("_unnamed_cfstring_")) {
      Value *nsstring_value = value_symbol.second;

      GlobalVariable *nsstring_global =
          dyn_cast<GlobalVariable>(nsstring_value);

      if (!nsstring_global) {
        LLDB_LOG(log, "NSString variable is not a GlobalVariable");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string is not a global variable\n");

        return false;
      }

      if (!nsstring_global->hasInitializer()) {
        LLDB_LOG(log, "NSString variable does not have an initializer");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string does not have an initializer\n");

        return false;
      }

      ConstantStruct *nsstring_struct =
          dyn_cast<ConstantStruct>(nsstring_global->getInitializer());

      if (!nsstring_struct) {
        LLDB_LOG(log,
                 "NSString variable's initializer is not a ConstantStruct");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string is not a structure constant\n");

        return false;
      }

      // We expect the following structure:
      //
      // struct {
      //   int *isa;
      //   int flags;
      //   char *str;
      //   long length;
      // };

      if (nsstring_struct->getNumOperands() != 4) {

        LLDB_LOG(log,
                 "NSString variable's initializer structure has an "
                 "unexpected number of members.  Should be 4, is {0}",
                 nsstring_struct->getNumOperands());

        m_error_stream.Printf("Internal error [IRForTarget]: The struct for an "
                              "Objective-C constant string is not as "
                              "expected\n");

        return false;
      }

      Constant *nsstring_member = nsstring_struct->getOperand(2);

      if (!nsstring_member) {
        LLDB_LOG(log, "NSString initializer's str element was empty");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string does not have a string "
                              "initializer\n");

        return false;
      }

      ConstantExpr *nsstring_expr = dyn_cast<ConstantExpr>(nsstring_member);

      if (!nsstring_expr) {
        LLDB_LOG(log,
                 "NSString initializer's str element is not a ConstantExpr");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string's string initializer is not "
                              "constant\n");

        return false;
      }

      GlobalVariable *cstr_global = nullptr;

      if (nsstring_expr->getOpcode() == Instruction::GetElementPtr) {
        Constant *nsstring_cstr = nsstring_expr->getOperand(0);
        cstr_global = dyn_cast<GlobalVariable>(nsstring_cstr);
      } else if (nsstring_expr->getOpcode() == Instruction::BitCast) {
        Constant *nsstring_cstr = nsstring_expr->getOperand(0);
        cstr_global = dyn_cast<GlobalVariable>(nsstring_cstr);
      }

      if (!cstr_global) {
        LLDB_LOG(log,
                 "NSString initializer's str element is not a GlobalVariable");

        m_error_stream.Printf("Internal error [IRForTarget]: Unhandled"
                              "constant string initializer\n");

        return false;
      }

      if (!cstr_global->hasInitializer()) {
        LLDB_LOG(log, "NSString initializer's str element does not have an "
                      "initializer");

        m_error_stream.Printf("Internal error [IRForTarget]: An Objective-C "
                              "constant string's string initializer doesn't "
                              "point to initialized data\n");

        return false;
      }

      /*
      if (!cstr_array)
      {
          if (log)
              log->PutCString("NSString initializer's str element is not a
      ConstantArray");

          if (m_error_stream)
              m_error_stream.Printf("Internal error [IRForTarget]: An
      Objective-C constant string's string initializer doesn't point to an
      array\n");

          return false;
      }

      if (!cstr_array->isCString())
      {
          if (log)
              log->PutCString("NSString initializer's str element is not a C
      string array");

          if (m_error_stream)
              m_error_stream.Printf("Internal error [IRForTarget]: An
      Objective-C constant string's string initializer doesn't point to a C
      string\n");

          return false;
      }
      */

      ConstantDataArray *cstr_array =
          dyn_cast<ConstantDataArray>(cstr_global->getInitializer());

      if (cstr_array)
        LLDB_LOG(log, "Found NSString constant {0}, which contains \"{1}\"",
                 value_name, cstr_array->getAsString());
      else
        LLDB_LOG(log, "Found NSString constant {0}, which contains \"\"",
                 value_name);

      if (!cstr_array)
        cstr_global = nullptr;

      if (!RewriteObjCConstString(nsstring_global, cstr_global)) {
        LLDB_LOG(log, "Error rewriting the constant string");

        // We don't print an error message here because RewriteObjCConstString
        // has done so for us.

        return false;
      }
    }
  }

  for (StringMapEntry<llvm::Value *> &value_symbol : value_symbol_table) {
    llvm::StringRef value_name = value_symbol.first();

    if (value_name == "__CFConstantStringClassReference") {
      GlobalVariable *gv = dyn_cast<GlobalVariable>(value_symbol.second);

      if (!gv) {
        LLDB_LOG(log,
                 "__CFConstantStringClassReference is not a global variable");

        m_error_stream.Printf("Internal error [IRForTarget]: Found a "
                              "CFConstantStringClassReference, but it is not a "
                              "global object\n");

        return false;
      }

      gv->eraseFromParent();

      break;
    }
  }

  return true;
}

static bool IsObjCSelectorRef(Value *value) {
  GlobalVariable *global_variable = dyn_cast<GlobalVariable>(value);

  return !(!global_variable || !global_variable->hasName() ||
           !global_variable->getName().startswith("OBJC_SELECTOR_REFERENCES_"));
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::RewriteObjCSelector(Instruction *selector_load) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  LoadInst *load = dyn_cast<LoadInst>(selector_load);

  if (!load)
    return false;

  // Unpack the message name from the selector.  In LLVM IR, an objc_msgSend
  // gets represented as
  //
  // %tmp     = load i8** @"OBJC_SELECTOR_REFERENCES_" ; <i8*> %call    = call
  // i8* (i8*, i8*, ...)* @objc_msgSend(i8* %obj, i8* %tmp, ...) ; <i8*>
  //
  // where %obj is the object pointer and %tmp is the selector.
  //
  // @"OBJC_SELECTOR_REFERENCES_" is a pointer to a character array called
  // @"\01L_OBJC_llvm_moduleETH_VAR_NAllvm_moduleE_".
  // @"\01L_OBJC_llvm_moduleETH_VAR_NAllvm_moduleE_" contains the string.

  // Find the pointer's initializer (a ConstantExpr with opcode GetElementPtr)
  // and get the string from its target

  GlobalVariable *_objc_selector_references_ =
      dyn_cast<GlobalVariable>(load->getPointerOperand());

  if (!_objc_selector_references_ ||
      !_objc_selector_references_->hasInitializer())
    return false;

  Constant *osr_initializer = _objc_selector_references_->getInitializer();

  ConstantExpr *osr_initializer_expr = dyn_cast<ConstantExpr>(osr_initializer);

  if (!osr_initializer_expr ||
      osr_initializer_expr->getOpcode() != Instruction::GetElementPtr)
    return false;

  Value *osr_initializer_base = osr_initializer_expr->getOperand(0);

  if (!osr_initializer_base)
    return false;

  // Find the string's initializer (a ConstantArray) and get the string from it

  GlobalVariable *_objc_meth_var_name_ =
      dyn_cast<GlobalVariable>(osr_initializer_base);

  if (!_objc_meth_var_name_ || !_objc_meth_var_name_->hasInitializer())
    return false;

  Constant *omvn_initializer = _objc_meth_var_name_->getInitializer();

  ConstantDataArray *omvn_initializer_array =
      dyn_cast<ConstantDataArray>(omvn_initializer);

  if (!omvn_initializer_array->isString())
    return false;

  std::string omvn_initializer_string = omvn_initializer_array->getAsString();

  LLDB_LOG(log, "Found Objective-C selector reference \"{0}\"",
           omvn_initializer_string);

  // Construct a call to sel_registerName

  if (!m_sel_registerName) {
    lldb::addr_t sel_registerName_addr;

    bool missing_weak = false;
    static lldb_private::ConstString g_sel_registerName_str("sel_registerName");
    sel_registerName_addr = m_execution_unit.FindSymbol(g_sel_registerName_str,
                                                        missing_weak);
    if (sel_registerName_addr == LLDB_INVALID_ADDRESS || missing_weak)
      return false;

    LLDB_LOG(log, "Found sel_registerName at {0}", sel_registerName_addr);

    // Build the function type: struct objc_selector
    // *sel_registerName(uint8_t*)

    // The below code would be "more correct," but in actuality what's required
    // is uint8_t*
    // Type *sel_type = StructType::get(m_module->getContext());
    // Type *sel_ptr_type = PointerType::getUnqual(sel_type);
    Type *sel_ptr_type = Type::getInt8PtrTy(m_module->getContext());

    Type *type_array[1];

    type_array[0] = llvm::Type::getInt8PtrTy(m_module->getContext());

    ArrayRef<Type *> srN_arg_types(type_array, 1);

    llvm::FunctionType *srN_type =
        FunctionType::get(sel_ptr_type, srN_arg_types, false);

    // Build the constant containing the pointer to the function
    PointerType *srN_ptr_ty = PointerType::getUnqual(srN_type);
    Constant *srN_addr_int =
        ConstantInt::get(m_intptr_ty, sel_registerName_addr, false);
    m_sel_registerName = {srN_type,
                          ConstantExpr::getIntToPtr(srN_addr_int, srN_ptr_ty)};
  }

  Value *argument_array[1];

  Constant *omvn_pointer = ConstantExpr::getBitCast(
      _objc_meth_var_name_, Type::getInt8PtrTy(m_module->getContext()));

  argument_array[0] = omvn_pointer;

  ArrayRef<Value *> srN_arguments(argument_array, 1);

  CallInst *srN_call = CallInst::Create(m_sel_registerName, srN_arguments,
                                        "sel_registerName", selector_load);

  // Replace the load with the call in all users

  selector_load->replaceAllUsesWith(srN_call);

  selector_load->eraseFromParent();

  return true;
}

bool IRForTarget::RewriteObjCSelectors(BasicBlock &basic_block) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  InstrList selector_loads;

  for (Instruction &inst : basic_block) {
    if (LoadInst *load = dyn_cast<LoadInst>(&inst))
      if (IsObjCSelectorRef(load->getPointerOperand()))
        selector_loads.push_back(&inst);
  }

  for (Instruction *inst : selector_loads) {
    if (!RewriteObjCSelector(inst)) {
      m_error_stream.Printf("Internal error [IRForTarget]: Couldn't change a "
                            "static reference to an Objective-C selector to a "
                            "dynamic reference\n");

      LLDB_LOG(log, "Couldn't rewrite a reference to an Objective-C selector");

      return false;
    }
  }

  return true;
}

static bool IsObjCClassReference(Value *value) {
  GlobalVariable *global_variable = dyn_cast<GlobalVariable>(value);

  return !(!global_variable || !global_variable->hasName() ||
           !global_variable->getName().startswith("OBJC_CLASS_REFERENCES_"));
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::RewriteObjCClassReference(Instruction *class_load) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  LoadInst *load = dyn_cast<LoadInst>(class_load);

  if (!load)
    return false;

  // Unpack the class name from the reference.  In LLVM IR, a reference to an
  // Objective-C class gets represented as
  //
  // %tmp     = load %struct._objc_class*,
  //            %struct._objc_class** @OBJC_CLASS_REFERENCES_, align 4
  //
  // @"OBJC_CLASS_REFERENCES_ is a bitcast of a character array called
  // @OBJC_CLASS_NAME_. @OBJC_CLASS_NAME contains the string.

  // Find the pointer's initializer (a ConstantExpr with opcode BitCast) and
  // get the string from its target

  GlobalVariable *_objc_class_references_ =
      dyn_cast<GlobalVariable>(load->getPointerOperand());

  if (!_objc_class_references_ ||
      !_objc_class_references_->hasInitializer())
    return false;

  Constant *ocr_initializer = _objc_class_references_->getInitializer();

  ConstantExpr *ocr_initializer_expr = dyn_cast<ConstantExpr>(ocr_initializer);

  if (!ocr_initializer_expr ||
      ocr_initializer_expr->getOpcode() != Instruction::BitCast)
    return false;

  Value *ocr_initializer_base = ocr_initializer_expr->getOperand(0);

  if (!ocr_initializer_base)
    return false;

  // Find the string's initializer (a ConstantArray) and get the string from it

  GlobalVariable *_objc_class_name_ =
      dyn_cast<GlobalVariable>(ocr_initializer_base);

  if (!_objc_class_name_ || !_objc_class_name_->hasInitializer())
    return false;

  Constant *ocn_initializer = _objc_class_name_->getInitializer();

  ConstantDataArray *ocn_initializer_array =
      dyn_cast<ConstantDataArray>(ocn_initializer);

  if (!ocn_initializer_array->isString())
    return false;

  std::string ocn_initializer_string = ocn_initializer_array->getAsString();

  LLDB_LOG(log, "Found Objective-C class reference \"{0}\"",
           ocn_initializer_string);

  // Construct a call to objc_getClass

  if (!m_objc_getClass) {
    lldb::addr_t objc_getClass_addr;

    bool missing_weak = false;
    static lldb_private::ConstString g_objc_getClass_str("objc_getClass");
    objc_getClass_addr = m_execution_unit.FindSymbol(g_objc_getClass_str,
                                                     missing_weak);
    if (objc_getClass_addr == LLDB_INVALID_ADDRESS || missing_weak)
      return false;

    LLDB_LOG(log, "Found objc_getClass at {0}", objc_getClass_addr);

    // Build the function type: %struct._objc_class *objc_getClass(i8*)

    Type *class_type = load->getType();
    Type *type_array[1];
    type_array[0] = llvm::Type::getInt8PtrTy(m_module->getContext());

    ArrayRef<Type *> ogC_arg_types(type_array, 1);

    llvm::FunctionType *ogC_type =
        FunctionType::get(class_type, ogC_arg_types, false);

    // Build the constant containing the pointer to the function
    PointerType *ogC_ptr_ty = PointerType::getUnqual(ogC_type);
    Constant *ogC_addr_int =
        ConstantInt::get(m_intptr_ty, objc_getClass_addr, false);
    m_objc_getClass = {ogC_type,
                       ConstantExpr::getIntToPtr(ogC_addr_int, ogC_ptr_ty)};
  }

  Value *argument_array[1];

  Constant *ocn_pointer = ConstantExpr::getBitCast(
      _objc_class_name_, Type::getInt8PtrTy(m_module->getContext()));

  argument_array[0] = ocn_pointer;

  ArrayRef<Value *> ogC_arguments(argument_array, 1);

  CallInst *ogC_call = CallInst::Create(m_objc_getClass, ogC_arguments,
                                        "objc_getClass", class_load);

  // Replace the load with the call in all users

  class_load->replaceAllUsesWith(ogC_call);

  class_load->eraseFromParent();

  return true;
}

bool IRForTarget::RewriteObjCClassReferences(BasicBlock &basic_block) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  InstrList class_loads;

  for (Instruction &inst : basic_block) {
    if (LoadInst *load = dyn_cast<LoadInst>(&inst))
      if (IsObjCClassReference(load->getPointerOperand()))
        class_loads.push_back(&inst);
  }

  for (Instruction *inst : class_loads) {
    if (!RewriteObjCClassReference(inst)) {
      m_error_stream.Printf("Internal error [IRForTarget]: Couldn't change a "
                            "static reference to an Objective-C class to a "
                            "dynamic reference\n");

      LLDB_LOG(log, "Couldn't rewrite a reference to an Objective-C class");

      return false;
    }
  }

  return true;
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::RewritePersistentAlloc(llvm::Instruction *persistent_alloc) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  AllocaInst *alloc = dyn_cast<AllocaInst>(persistent_alloc);

  MDNode *alloc_md = alloc->getMetadata("clang.decl.ptr");

  if (!alloc_md || !alloc_md->getNumOperands())
    return false;

  ConstantInt *constant_int =
      mdconst::dyn_extract<ConstantInt>(alloc_md->getOperand(0));

  if (!constant_int)
    return false;

  // We attempt to register this as a new persistent variable with the DeclMap.

  uintptr_t ptr = constant_int->getZExtValue();

  clang::VarDecl *decl = reinterpret_cast<clang::VarDecl *>(ptr);

  lldb_private::TypeFromParser result_decl_type(
      decl->getType().getAsOpaquePtr(),
      lldb_private::ClangASTContext::GetASTContext(&decl->getASTContext()));

  StringRef decl_name(decl->getName());
  lldb_private::ConstString persistent_variable_name(decl_name.data(),
                                                     decl_name.size());
  if (!m_decl_map->AddPersistentVariable(decl, persistent_variable_name,
                                         result_decl_type, false, false))
    return false;

  GlobalVariable *persistent_global = new GlobalVariable(
      (*m_module), alloc->getType(), false,  /* not constant */
      GlobalValue::ExternalLinkage, nullptr, /* no initializer */
      alloc->getName().str());

  // What we're going to do here is make believe this was a regular old
  // external variable.  That means we need to make the metadata valid.

  NamedMDNode *named_metadata =
      m_module->getOrInsertNamedMetadata("clang.global.decl.ptrs");

  llvm::Metadata *values[2];
  values[0] = ConstantAsMetadata::get(persistent_global);
  values[1] = ConstantAsMetadata::get(constant_int);

  ArrayRef<llvm::Metadata *> value_ref(values, 2);

  MDNode *persistent_global_md = MDNode::get(m_module->getContext(), value_ref);
  named_metadata->addOperand(persistent_global_md);

  // Now, since the variable is a pointer variable, we will drop in a load of
  // that pointer variable.

  LoadInst *persistent_load = new LoadInst(persistent_global, "", alloc);

  LLDB_LOG(log, "Replacing \"{0}\" with \"{1}\"", PrintValue(alloc),
           PrintValue(persistent_load));

  alloc->replaceAllUsesWith(persistent_load);
  alloc->eraseFromParent();

  return true;
}

bool IRForTarget::RewritePersistentAllocs(llvm::BasicBlock &basic_block) {
  if (!m_resolve_vars)
    return true;

  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  InstrList pvar_allocs;

  for (Instruction &inst : basic_block) {

    if (AllocaInst *alloc = dyn_cast<AllocaInst>(&inst)) {
      llvm::StringRef alloc_name = alloc->getName();

      if (alloc_name.startswith("$") && !alloc_name.startswith("$__lldb")) {
        if (alloc_name.find_first_of("0123456789") == 1) {
          LLDB_LOG(log, "Rejecting a numeric persistent variable.");

          m_error_stream.Printf("Error [IRForTarget]: Names starting with $0, "
                                "$1, ... are reserved for use as result "
                                "names\n");

          return false;
        }

        pvar_allocs.push_back(alloc);
      }
    }
  }

  for (Instruction *inst : pvar_allocs) {
    if (!RewritePersistentAlloc(inst)) {
      m_error_stream.Printf("Internal error [IRForTarget]: Couldn't rewrite "
                            "the creation of a persistent variable\n");

      LLDB_LOG(log, "Couldn't rewrite the creation of a persistent variable");

      return false;
    }
  }

  return true;
}

bool IRForTarget::MaterializeInitializer(uint8_t *data, Constant *initializer) {
  if (!initializer)
    return true;

  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  LLDB_LOGV(log, "  MaterializeInitializer({0}, {1})", (void *)data,
            PrintValue(initializer));

  Type *initializer_type = initializer->getType();

  if (ConstantInt *int_initializer = dyn_cast<ConstantInt>(initializer)) {
    size_t constant_size = m_target_data->getTypeStoreSize(initializer_type);
    lldb_private::Scalar scalar = int_initializer->getValue().zextOrTrunc(
        llvm::NextPowerOf2(constant_size) * 8);

    lldb_private::Status get_data_error;
    return scalar.GetAsMemoryData(data, constant_size,
                                  lldb_private::endian::InlHostByteOrder(),
                                  get_data_error) != 0;
  } else if (ConstantDataArray *array_initializer =
                 dyn_cast<ConstantDataArray>(initializer)) {
    if (array_initializer->isString()) {
      std::string array_initializer_string = array_initializer->getAsString();
      memcpy(data, array_initializer_string.c_str(),
             m_target_data->getTypeStoreSize(initializer_type));
    } else {
      ArrayType *array_initializer_type = array_initializer->getType();
      Type *array_element_type = array_initializer_type->getElementType();

      size_t element_size = m_target_data->getTypeAllocSize(array_element_type);

      for (unsigned i = 0; i < array_initializer->getNumOperands(); ++i) {
        Value *operand_value = array_initializer->getOperand(i);
        Constant *operand_constant = dyn_cast<Constant>(operand_value);

        if (!operand_constant)
          return false;

        if (!MaterializeInitializer(data + (i * element_size),
                                    operand_constant))
          return false;
      }
    }
    return true;
  } else if (ConstantStruct *struct_initializer =
                 dyn_cast<ConstantStruct>(initializer)) {
    StructType *struct_initializer_type = struct_initializer->getType();
    const StructLayout *struct_layout =
        m_target_data->getStructLayout(struct_initializer_type);

    for (unsigned i = 0; i < struct_initializer->getNumOperands(); ++i) {
      if (!MaterializeInitializer(data + struct_layout->getElementOffset(i),
                                  struct_initializer->getOperand(i)))
        return false;
    }
    return true;
  } else if (isa<ConstantAggregateZero>(initializer)) {
    memset(data, 0, m_target_data->getTypeStoreSize(initializer_type));
    return true;
  }
  return false;
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::MaybeHandleVariable(Value *llvm_value_ptr) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  LLDB_LOG(log, "MaybeHandleVariable ({0})", PrintValue(llvm_value_ptr));

  if (ConstantExpr *constant_expr = dyn_cast<ConstantExpr>(llvm_value_ptr)) {
    switch (constant_expr->getOpcode()) {
    default:
      break;
    case Instruction::GetElementPtr:
    case Instruction::BitCast:
      Value *s = constant_expr->getOperand(0);
      if (!MaybeHandleVariable(s))
        return false;
    }
  } else if (GlobalVariable *global_variable =
                 dyn_cast<GlobalVariable>(llvm_value_ptr)) {
    if (!GlobalValue::isExternalLinkage(global_variable->getLinkage()))
      return true;

    clang::NamedDecl *named_decl = DeclForGlobal(global_variable);

    if (!named_decl) {
      LLDB_LOG(log, "Found global variable \"{0}\" without metadata",
               global_variable->getName());

      return true;
    }

    llvm::StringRef name(named_decl->getName());

    clang::ValueDecl *value_decl = dyn_cast<clang::ValueDecl>(named_decl);
    if (value_decl == nullptr)
      return false;

    lldb_private::CompilerType compiler_type(&value_decl->getASTContext(),
                                             value_decl->getType());

    const Type *value_type = nullptr;

    if (name.startswith("$")) {
      // The $__lldb_expr_result name indicates the return value has allocated
      // as a static variable.  Per the comment at
      // ASTResultSynthesizer::SynthesizeBodyResult, accesses to this static
      // variable need to be redirected to the result of dereferencing a
      // pointer that is passed in as one of the arguments.
      //
      // Consequently, when reporting the size of the type, we report a pointer
      // type pointing to the type of $__lldb_expr_result, not the type itself.
      //
      // We also do this for any user-declared persistent variables.
      compiler_type = compiler_type.GetPointerType();
      value_type = PointerType::get(global_variable->getType(), 0);
    } else {
      value_type = global_variable->getType();
    }

    llvm::Optional<uint64_t> value_size = compiler_type.GetByteSize(nullptr);
    if (!value_size)
      return false;
    llvm::Optional<size_t> opt_alignment = compiler_type.GetTypeBitAlign();
    if (!opt_alignment)
      return false;
    lldb::offset_t value_alignment = (*opt_alignment + 7ull) / 8ull;

    LLDB_LOG(log,
             "Type of \"{0}\" is [clang \"{1}\", llvm \"{2}\"] [size {3}, "
             "align {4}]",
             name,
             lldb_private::ClangUtil::GetQualType(compiler_type).getAsString(),
             PrintType(value_type), *value_size, value_alignment);

    if (named_decl)
      m_decl_map->AddValueToStruct(named_decl, lldb_private::ConstString(name),
                                   llvm_value_ptr, *value_size,
                                   value_alignment);
  } else if (dyn_cast<llvm::Function>(llvm_value_ptr)) {
    LLDB_LOG(log, "Function pointers aren't handled right now");

    return false;
  }

  return true;
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::HandleSymbol(Value *symbol) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  lldb_private::ConstString name(symbol->getName().str().c_str());

  lldb::addr_t symbol_addr =
      m_decl_map->GetSymbolAddress(name, lldb::eSymbolTypeAny);

  if (symbol_addr == LLDB_INVALID_ADDRESS) {
    LLDB_LOG(log, "Symbol \"{0}\" had no address", name);

    return false;
  }

  LLDB_LOG(log, "Found \"{0}\" at {1}", name, symbol_addr);

  Type *symbol_type = symbol->getType();

  Constant *symbol_addr_int = ConstantInt::get(m_intptr_ty, symbol_addr, false);

  Value *symbol_addr_ptr =
      ConstantExpr::getIntToPtr(symbol_addr_int, symbol_type);

  LLDB_LOG(log, "Replacing {0} with {1}", PrintValue(symbol),
           PrintValue(symbol_addr_ptr));

  symbol->replaceAllUsesWith(symbol_addr_ptr);

  return true;
}

bool IRForTarget::MaybeHandleCallArguments(CallInst *Old) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  LLDB_LOG(log, "MaybeHandleCallArguments({0})", PrintValue(Old));

  for (unsigned op_index = 0, num_ops = Old->getNumArgOperands();
       op_index < num_ops; ++op_index)
    // conservatively believe that this is a store
    if (!MaybeHandleVariable(Old->getArgOperand(op_index))) {
      m_error_stream.Printf("Internal error [IRForTarget]: Couldn't rewrite "
                            "one of the arguments of a function call.\n");

      return false;
    }

  return true;
}

bool IRForTarget::HandleObjCClass(Value *classlist_reference) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  GlobalVariable *global_variable =
      dyn_cast<GlobalVariable>(classlist_reference);

  if (!global_variable)
    return false;

  Constant *initializer = global_variable->getInitializer();

  if (!initializer)
    return false;

  if (!initializer->hasName())
    return false;

  StringRef name(initializer->getName());
  lldb_private::ConstString name_cstr(name.str().c_str());
  lldb::addr_t class_ptr =
      m_decl_map->GetSymbolAddress(name_cstr, lldb::eSymbolTypeObjCClass);

  LLDB_LOG(log, "Found reference to Objective-C class {0} ({1})", name,
           (unsigned long long)class_ptr);

  if (class_ptr == LLDB_INVALID_ADDRESS)
    return false;

  if (global_variable->use_empty())
    return false;

  SmallVector<LoadInst *, 2> load_instructions;

  for (llvm::User *u : global_variable->users()) {
    if (LoadInst *load_instruction = dyn_cast<LoadInst>(u))
      load_instructions.push_back(load_instruction);
  }

  if (load_instructions.empty())
    return false;

  Constant *class_addr = ConstantInt::get(m_intptr_ty, (uint64_t)class_ptr);

  for (LoadInst *load_instruction : load_instructions) {
    Constant *class_bitcast =
        ConstantExpr::getIntToPtr(class_addr, load_instruction->getType());

    load_instruction->replaceAllUsesWith(class_bitcast);

    load_instruction->eraseFromParent();
  }

  return true;
}

bool IRForTarget::RemoveCXAAtExit(BasicBlock &basic_block) {
  std::vector<CallInst *> calls_to_remove;

  for (Instruction &inst : basic_block) {
    CallInst *call = dyn_cast<CallInst>(&inst);

    // MaybeHandleCallArguments handles error reporting; we are silent here
    if (!call)
      continue;

    bool remove = false;

    llvm::Function *func = call->getCalledFunction();

    if (func && func->getName() == "__cxa_atexit")
      remove = true;

    llvm::Value *val = call->getCalledValue();

    if (val && val->getName() == "__cxa_atexit")
      remove = true;

    if (remove)
      calls_to_remove.push_back(call);
  }

  for (CallInst *ci : calls_to_remove)
    ci->eraseFromParent();

  return true;
}

bool IRForTarget::ResolveCalls(BasicBlock &basic_block) {
  // Prepare the current basic block for execution in the remote process

  for (Instruction &inst : basic_block) {
    CallInst *call = dyn_cast<CallInst>(&inst);

    // MaybeHandleCallArguments handles error reporting; we are silent here
    if (call && !MaybeHandleCallArguments(call))
      return false;
  }

  return true;
}

bool IRForTarget::ResolveExternals(Function &llvm_function) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  for (GlobalVariable &global_var : m_module->globals()) {
    llvm::StringRef global_name = global_var.getName();

    LLDB_LOG(log, "Examining {0}, DeclForGlobalValue returns {1}", global_name,
             static_cast<void *>(DeclForGlobal(&global_var)));

    if (global_name.startswith("OBJC_IVAR")) {
      if (!HandleSymbol(&global_var)) {
        m_error_stream.Format("Error [IRForTarget]: Couldn't find Objective-C "
                              "indirect ivar symbol {0}\n",
                              global_name);

        return false;
      }
    } else if (global_name.contains("OBJC_CLASSLIST_REFERENCES_$")) {
      if (!HandleObjCClass(&global_var)) {
        m_error_stream.Printf("Error [IRForTarget]: Couldn't resolve the class "
                              "for an Objective-C static method call\n");

        return false;
      }
    } else if (global_name.contains("OBJC_CLASSLIST_SUP_REFS_$")) {
      if (!HandleObjCClass(&global_var)) {
        m_error_stream.Printf("Error [IRForTarget]: Couldn't resolve the class "
                              "for an Objective-C static method call\n");

        return false;
      }
    } else if (DeclForGlobal(&global_var)) {
      if (!MaybeHandleVariable(&global_var)) {
        m_error_stream.Format("Internal error [IRForTarget]: Couldn't rewrite "
                              "external variable {0}\n",
                              global_name);

        return false;
      }
    }
  }

  return true;
}

static bool isGuardVariableRef(Value *V) {
  Constant *Old = dyn_cast<Constant>(V);

  if (!Old)
    return false;

  if (auto CE = dyn_cast<ConstantExpr>(V)) {
    if (CE->getOpcode() != Instruction::BitCast)
      return false;

    Old = CE->getOperand(0);
  }

  GlobalVariable *GV = dyn_cast<GlobalVariable>(Old);

  if (!GV || !GV->hasName() || !isGuardVariableSymbol(GV->getName()))
    return false;

  return true;
}

void IRForTarget::TurnGuardLoadIntoZero(llvm::Instruction *guard_load) {
  Constant *zero(Constant::getNullValue(guard_load->getType()));
  guard_load->replaceAllUsesWith(zero);
  guard_load->eraseFromParent();
}

static void ExciseGuardStore(Instruction *guard_store) {
  guard_store->eraseFromParent();
}

bool IRForTarget::RemoveGuards(BasicBlock &basic_block) {
  // Eliminate any reference to guard variables found.

  InstrList guard_loads;
  InstrList guard_stores;

  for (Instruction &inst : basic_block) {

    if (LoadInst *load = dyn_cast<LoadInst>(&inst))
      if (isGuardVariableRef(load->getPointerOperand()))
        guard_loads.push_back(&inst);

    if (StoreInst *store = dyn_cast<StoreInst>(&inst))
      if (isGuardVariableRef(store->getPointerOperand()))
        guard_stores.push_back(&inst);
  }

  for (Instruction *inst : guard_loads)
    TurnGuardLoadIntoZero(inst);

  for (Instruction *inst : guard_stores)
    ExciseGuardStore(inst);

  return true;
}

// This function does not report errors; its callers are responsible.
bool IRForTarget::UnfoldConstant(Constant *old_constant,
                                 llvm::Function *llvm_function,
                                 FunctionValueCache &value_maker,
                                 FunctionValueCache &entry_instruction_finder,
                                 lldb_private::Stream &error_stream) {
  SmallVector<User *, 16> users;

  // We do this because the use list might change, invalidating our iterator.
  // Much better to keep a work list ourselves.
  for (llvm::User *u : old_constant->users())
    users.push_back(u);

  for (size_t i = 0; i < users.size(); ++i) {
    User *user = users[i];

    if (Constant *constant = dyn_cast<Constant>(user)) {
      // synthesize a new non-constant equivalent of the constant

      if (ConstantExpr *constant_expr = dyn_cast<ConstantExpr>(constant)) {
        switch (constant_expr->getOpcode()) {
        default:
          error_stream.Printf("error [IRForTarget internal]: Unhandled "
                              "constant expression type: \"%s\"",
                              PrintValue(constant_expr).c_str());
          return false;
        case Instruction::BitCast: {
          FunctionValueCache bit_cast_maker(
              [&value_maker, &entry_instruction_finder, old_constant,
               constant_expr](llvm::Function *function) -> llvm::Value * {
                // UnaryExpr
                //   OperandList[0] is value

                if (constant_expr->getOperand(0) != old_constant)
                  return constant_expr;

                return new BitCastInst(
                    value_maker.GetValue(function), constant_expr->getType(),
                    "", llvm::cast<Instruction>(
                            entry_instruction_finder.GetValue(function)));
              });

          if (!UnfoldConstant(constant_expr, llvm_function, bit_cast_maker,
                              entry_instruction_finder, error_stream))
            return false;
        } break;
        case Instruction::GetElementPtr: {
          // GetElementPtrConstantExpr
          //   OperandList[0] is base
          //   OperandList[1]... are indices

          FunctionValueCache get_element_pointer_maker(
              [&value_maker, &entry_instruction_finder, old_constant,
               constant_expr](llvm::Function *function) -> llvm::Value * {
                Value *ptr = constant_expr->getOperand(0);

                if (ptr == old_constant)
                  ptr = value_maker.GetValue(function);

                std::vector<Value *> index_vector;

                unsigned operand_index;
                unsigned num_operands = constant_expr->getNumOperands();

                for (operand_index = 1; operand_index < num_operands;
                     ++operand_index) {
                  Value *operand = constant_expr->getOperand(operand_index);

                  if (operand == old_constant)
                    operand = value_maker.GetValue(function);

                  index_vector.push_back(operand);
                }

                ArrayRef<Value *> indices(index_vector);

                return GetElementPtrInst::Create(
                    nullptr, ptr, indices, "",
                    llvm::cast<Instruction>(
                        entry_instruction_finder.GetValue(function)));
              });

          if (!UnfoldConstant(constant_expr, llvm_function,
                              get_element_pointer_maker,
                              entry_instruction_finder, error_stream))
            return false;
        } break;
        }
      } else {
        error_stream.Printf(
            "error [IRForTarget internal]: Unhandled constant type: \"%s\"",
            PrintValue(constant).c_str());
        return false;
      }
    } else {
      if (Instruction *inst = llvm::dyn_cast<Instruction>(user)) {
        if (llvm_function && inst->getParent()->getParent() != llvm_function) {
          error_stream.PutCString("error: Capturing non-local variables in "
                                  "expressions is unsupported.\n");
          return false;
        }
        inst->replaceUsesOfWith(
            old_constant, value_maker.GetValue(inst->getParent()->getParent()));
      } else {
        error_stream.Printf(
            "error [IRForTarget internal]: Unhandled non-constant type: \"%s\"",
            PrintValue(user).c_str());
        return false;
      }
    }
  }

  if (!isa<GlobalValue>(old_constant)) {
    old_constant->destroyConstant();
  }

  return true;
}

bool IRForTarget::ReplaceVariables(Function &llvm_function) {
  if (!m_resolve_vars)
    return true;

  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  m_decl_map->DoStructLayout();

  LLDB_LOG(log, "Element arrangement:");

  uint32_t num_elements;
  uint32_t element_index;

  size_t size;
  lldb::offset_t alignment;

  if (!m_decl_map->GetStructInfo(num_elements, size, alignment))
    return false;

  Function::arg_iterator iter(llvm_function.arg_begin());

  if (iter == llvm_function.arg_end()) {
    m_error_stream.Printf("Internal error [IRForTarget]: Wrapper takes no "
                          "arguments (should take at least a struct pointer)");

    return false;
  }

  Argument *argument = &*iter;

  if (argument->getName().equals("this")) {
    ++iter;

    if (iter == llvm_function.arg_end()) {
      m_error_stream.Printf("Internal error [IRForTarget]: Wrapper takes only "
                            "'this' argument (should take a struct pointer "
                            "too)");

      return false;
    }

    argument = &*iter;
  } else if (argument->getName().equals("self")) {
    ++iter;

    if (iter == llvm_function.arg_end()) {
      m_error_stream.Printf("Internal error [IRForTarget]: Wrapper takes only "
                            "'self' argument (should take '_cmd' and a struct "
                            "pointer too)");

      return false;
    }

    if (!iter->getName().equals("_cmd")) {
      m_error_stream.Format("Internal error [IRForTarget]: Wrapper takes '{0}' "
                            "after 'self' argument (should take '_cmd')",
                            iter->getName());

      return false;
    }

    ++iter;

    if (iter == llvm_function.arg_end()) {
      m_error_stream.Printf("Internal error [IRForTarget]: Wrapper takes only "
                            "'self' and '_cmd' arguments (should take a struct "
                            "pointer too)");

      return false;
    }

    argument = &*iter;
  }

  if (!argument->getName().equals("$__lldb_arg")) {
    m_error_stream.Format("Internal error [IRForTarget]: Wrapper takes an "
                          "argument named '{0}' instead of the struct pointer",
                          argument->getName());

    return false;
  }

  LLDB_LOG(log, "Arg: \"{0}\"", PrintValue(argument));

  BasicBlock &entry_block(llvm_function.getEntryBlock());
  Instruction *FirstEntryInstruction(entry_block.getFirstNonPHIOrDbg());

  if (!FirstEntryInstruction) {
    m_error_stream.Printf("Internal error [IRForTarget]: Couldn't find the "
                          "first instruction in the wrapper for use in "
                          "rewriting");

    return false;
  }

  LLVMContext &context(m_module->getContext());
  IntegerType *offset_type(Type::getInt32Ty(context));

  if (!offset_type) {
    m_error_stream.Printf(
        "Internal error [IRForTarget]: Couldn't produce an offset type");

    return false;
  }

  for (element_index = 0; element_index < num_elements; ++element_index) {
    const clang::NamedDecl *decl = nullptr;
    Value *value = nullptr;
    lldb::offset_t offset;
    lldb_private::ConstString name;

    if (!m_decl_map->GetStructElement(decl, value, offset, name,
                                      element_index)) {
      m_error_stream.Printf(
          "Internal error [IRForTarget]: Structure information is incomplete");

      return false;
    }

    LLDB_LOG(log, "  \"{0}\" (\"{1}\") placed at {2}", name,
             decl->getNameAsString(), offset);

    if (value) {
      LLDB_LOG(log, "    Replacing [{0}]", PrintValue(value));

      FunctionValueCache body_result_maker(
          [this, name, offset_type, offset, argument,
           value](llvm::Function *function) -> llvm::Value * {
            // Per the comment at ASTResultSynthesizer::SynthesizeBodyResult,
            // in cases where the result variable is an rvalue, we have to
            // synthesize a dereference of the appropriate structure entry in
            // order to produce the static variable that the AST thinks it is
            // accessing.

            llvm::Instruction *entry_instruction = llvm::cast<Instruction>(
                m_entry_instruction_finder.GetValue(function));

            ConstantInt *offset_int(
                ConstantInt::get(offset_type, offset, true));
            GetElementPtrInst *get_element_ptr = GetElementPtrInst::Create(
                nullptr, argument, offset_int, "", entry_instruction);

            if (name == m_result_name && !m_result_is_pointer) {
              BitCastInst *bit_cast = new BitCastInst(
                  get_element_ptr, value->getType()->getPointerTo(), "",
                  entry_instruction);

              LoadInst *load = new LoadInst(bit_cast, "", entry_instruction);

              return load;
            } else {
              BitCastInst *bit_cast = new BitCastInst(
                  get_element_ptr, value->getType(), "", entry_instruction);

              return bit_cast;
            }
          });

      if (Constant *constant = dyn_cast<Constant>(value)) {
        if (!UnfoldConstant(constant, &llvm_function, body_result_maker,
                            m_entry_instruction_finder, m_error_stream)) {
          return false;
        }
      } else if (Instruction *instruction = dyn_cast<Instruction>(value)) {
        if (instruction->getParent()->getParent() != &llvm_function) {
          m_error_stream.PutCString("error: Capturing non-local variables in "
                                    "expressions is unsupported.\n");
          return false;
        }
        value->replaceAllUsesWith(
            body_result_maker.GetValue(instruction->getParent()->getParent()));
      } else {
        LLDB_LOG(log, "Unhandled non-constant type: \"{0}\"",
                 PrintValue(value));
        return false;
      }

      if (GlobalVariable *var = dyn_cast<GlobalVariable>(value))
        var->eraseFromParent();
    }
  }

  LLDB_LOG(log, "Total structure [align {0}, size {1}]", (int64_t)alignment,
           (uint64_t)size);

  return true;
}

bool IRForTarget::runOnModule(Module &llvm_module) {
  lldb_private::Log *log(
      lldb_private::GetLogIfAllCategoriesSet(LIBLLDB_LOG_EXPRESSIONS));

  m_module = &llvm_module;
  m_target_data.reset(new DataLayout(m_module));
  m_intptr_ty = llvm::Type::getIntNTy(m_module->getContext(),
                                      m_target_data->getPointerSizeInBits());

  if (log) {
    std::string s;
    raw_string_ostream oss(s);

    m_module->print(oss, nullptr);

    oss.flush();

    LLDB_LOG(log, "Module as passed in to IRForTarget: \n\"{0}\"", s);
  }

  Function *const main_function =
      m_func_name.IsEmpty() ? nullptr
                            : m_module->getFunction(m_func_name.GetStringRef());

  if (!m_func_name.IsEmpty() && !main_function) {
    LLDB_LOG(log, "Couldn't find \"{0}()\" in the module", m_func_name);

    m_error_stream.Format("Internal error [IRForTarget]: Couldn't find wrapper "
                          "'{0}' in the module",
                          m_func_name);

    return false;
  }

  if (main_function) {
    if (!FixFunctionLinkage(*main_function)) {
      LLDB_LOG(log, "Couldn't fix the linkage for the function");

      return false;
    }
  }

  llvm::Type *int8_ty = Type::getInt8Ty(m_module->getContext());

  m_reloc_placeholder = new llvm::GlobalVariable(
      (*m_module), int8_ty, false /* IsConstant */,
      GlobalVariable::InternalLinkage, Constant::getNullValue(int8_ty),
      "reloc_placeholder", nullptr /* InsertBefore */,
      GlobalVariable::NotThreadLocal /* ThreadLocal */, 0 /* AddressSpace */);

  ////////////////////////////////////////////////////////////
  // Replace $__lldb_expr_result with a persistent variable
  //

  if (main_function) {
    if (!CreateResultVariable(*main_function)) {
      LLDB_LOG(log, "CreateResultVariable() failed");

      // CreateResultVariable() reports its own errors, so we don't do so here

      return false;
    }
  }

  if (log && log->GetVerbose()) {
    std::string s;
    raw_string_ostream oss(s);

    m_module->print(oss, nullptr);

    oss.flush();

    LLDB_LOG(log, "Module after creating the result variable: \n\"{0}\"", s);
  }

  for (llvm::Function &function : *m_module) {
    for (BasicBlock &bb : function) {
      if (!RemoveGuards(bb)) {
        LLDB_LOG(log, "RemoveGuards() failed");

        // RemoveGuards() reports its own errors, so we don't do so here

        return false;
      }

      if (!RewritePersistentAllocs(bb)) {
        LLDB_LOG(log, "RewritePersistentAllocs() failed");

        // RewritePersistentAllocs() reports its own errors, so we don't do so
        // here

        return false;
      }

      if (!RemoveCXAAtExit(bb)) {
        LLDB_LOG(log, "RemoveCXAAtExit() failed");

        // RemoveCXAAtExit() reports its own errors, so we don't do so here

        return false;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Fix all Objective-C constant strings to use NSStringWithCString:encoding:
  //

  if (!RewriteObjCConstStrings()) {
    LLDB_LOG(log, "RewriteObjCConstStrings() failed");

    // RewriteObjCConstStrings() reports its own errors, so we don't do so here

    return false;
  }

  for (llvm::Function &function : *m_module) {
    for (llvm::BasicBlock &bb : function) {
      if (!RewriteObjCSelectors(bb)) {
        LLDB_LOG(log, "RewriteObjCSelectors() failed");

        // RewriteObjCSelectors() reports its own errors, so we don't do so
        // here

        return false;
      }

      if (!RewriteObjCClassReferences(bb)) {
        LLDB_LOG(log, "RewriteObjCClassReferences() failed");

        // RewriteObjCClasses() reports its own errors, so we don't do so here

        return false;
      }
    }
  }

  for (llvm::Function &function : *m_module) {
    for (BasicBlock &bb : function) {
      if (!ResolveCalls(bb)) {
        LLDB_LOG(log, "ResolveCalls() failed");

        // ResolveCalls() reports its own errors, so we don't do so here

        return false;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Run function-level passes that only make sense on the main function
  //

  if (main_function) {
    if (!ResolveExternals(*main_function)) {
      LLDB_LOG(log, "ResolveExternals() failed");

      // ResolveExternals() reports its own errors, so we don't do so here

      return false;
    }

    if (!ReplaceVariables(*main_function)) {
      LLDB_LOG(log, "ReplaceVariables() failed");

      // ReplaceVariables() reports its own errors, so we don't do so here

      return false;
    }
  }

  if (log && log->GetVerbose()) {
    std::string s;
    raw_string_ostream oss(s);

    m_module->print(oss, nullptr);

    oss.flush();

    LLDB_LOG(log, "Module after preparing for execution: \n\"{0}\"", s);
  }

  return true;
}

void IRForTarget::assignPassManager(PMStack &pass_mgr_stack,
                                    PassManagerType pass_mgr_type) {}

PassManagerType IRForTarget::getPotentialPassManagerType() const {
  return PMT_ModulePassManager;
}
