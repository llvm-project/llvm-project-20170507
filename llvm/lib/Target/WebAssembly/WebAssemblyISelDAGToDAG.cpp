//- WebAssemblyISelDAGToDAG.cpp - A dag to dag inst selector for WebAssembly -//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines an instruction selector for the WebAssembly target.
///
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/WebAssemblyMCTargetDesc.h"
#include "WebAssembly.h"
#include "WebAssemblyTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/Function.h" // To access function attributes.
#include "llvm/Support/Debug.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "wasm-isel"

//===--------------------------------------------------------------------===//
/// WebAssembly-specific code to select WebAssembly machine instructions for
/// SelectionDAG operations.
///
namespace {
class WebAssemblyDAGToDAGISel final : public SelectionDAGISel {
  /// Keep a pointer to the WebAssemblySubtarget around so that we can make the
  /// right decision when generating code for different targets.
  const WebAssemblySubtarget *Subtarget;

  bool ForCodeSize;

public:
  WebAssemblyDAGToDAGISel(WebAssemblyTargetMachine &tm,
                          CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(tm, OptLevel), Subtarget(nullptr), ForCodeSize(false) {
  }

  StringRef getPassName() const override {
    return "WebAssembly Instruction Selection";
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    LLVM_DEBUG(dbgs() << "********** ISelDAGToDAG **********\n"
                         "********** Function: "
                      << MF.getName() << '\n');

    ForCodeSize = MF.getFunction().hasFnAttribute(Attribute::OptimizeForSize) ||
                  MF.getFunction().hasFnAttribute(Attribute::MinSize);
    Subtarget = &MF.getSubtarget<WebAssemblySubtarget>();
    return SelectionDAGISel::runOnMachineFunction(MF);
  }

  void Select(SDNode *Node) override;

  bool SelectInlineAsmMemoryOperand(const SDValue &Op, unsigned ConstraintID,
                                    std::vector<SDValue> &OutOps) override;

// Include the pieces autogenerated from the target description.
#include "WebAssemblyGenDAGISel.inc"

private:
  // add select functions here...
};
} // end anonymous namespace

void WebAssemblyDAGToDAGISel::Select(SDNode *Node) {
  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // Few custom selection stuff. If we need WebAssembly-specific selection,
  // uncomment this block add corresponding case statements.
  /*
  switch (Node->getOpcode()) {
  default:
    break;
  }
  */

  // Select the default instruction.
  SelectCode(Node);
}

bool WebAssemblyDAGToDAGISel::SelectInlineAsmMemoryOperand(
    const SDValue &Op, unsigned ConstraintID, std::vector<SDValue> &OutOps) {
  switch (ConstraintID) {
  case InlineAsm::Constraint_i:
  case InlineAsm::Constraint_m:
    // We just support simple memory operands that just have a single address
    // operand and need no special handling.
    OutOps.push_back(Op);
    return false;
  default:
    break;
  }

  return true;
}

/// This pass converts a legalized DAG into a WebAssembly-specific DAG, ready
/// for instruction scheduling.
FunctionPass *llvm::createWebAssemblyISelDag(WebAssemblyTargetMachine &TM,
                                             CodeGenOpt::Level OptLevel) {
  return new WebAssemblyDAGToDAGISel(TM, OptLevel);
}
