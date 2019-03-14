//==-- llvm/CodeGen/GlobalISel/Utils.h ---------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
/// \file This file declares the API of helper functions used throughout the
/// GlobalISel pipeline.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_GLOBALISEL_UTILS_H
#define LLVM_CODEGEN_GLOBALISEL_UTILS_H

#include "llvm/ADT/StringRef.h"

namespace llvm {

class AnalysisUsage;
class MachineFunction;
class MachineInstr;
class MachineOperand;
class MachineOptimizationRemarkEmitter;
class MachineOptimizationRemarkMissed;
class MachineRegisterInfo;
class MCInstrDesc;
class RegisterBankInfo;
class TargetInstrInfo;
class TargetPassConfig;
class TargetRegisterInfo;
class TargetRegisterClass;
class Twine;
class ConstantFP;
class APFloat;

/// Try to constrain Reg to the specified register class. If this fails,
/// create a new virtual register in the correct class and insert a COPY before
/// \p InsertPt. The debug location of \p InsertPt is used for the new copy.
///
/// \return The virtual register constrained to the right register class.
unsigned constrainRegToClass(MachineRegisterInfo &MRI,
                             const TargetInstrInfo &TII,
                             const RegisterBankInfo &RBI,
                             MachineInstr &InsertPt, unsigned Reg,
                             const TargetRegisterClass &RegClass);

/// Try to constrain Reg so that it is usable by argument OpIdx of the
/// provided MCInstrDesc \p II. If this fails, create a new virtual
/// register in the correct class and insert a COPY before \p InsertPt.
/// This is equivalent to constrainRegToClass() with RegClass obtained from the
/// MCInstrDesc. The debug location of \p InsertPt is used for the new copy.
///
/// \return The virtual register constrained to the right register class.
unsigned constrainOperandRegClass(const MachineFunction &MF,
                                  const TargetRegisterInfo &TRI,
                                  MachineRegisterInfo &MRI,
                                  const TargetInstrInfo &TII,
                                  const RegisterBankInfo &RBI,
                                  MachineInstr &InsertPt, const MCInstrDesc &II,
                                  const MachineOperand &RegMO, unsigned OpIdx);

/// Mutate the newly-selected instruction \p I to constrain its (possibly
/// generic) virtual register operands to the instruction's register class.
/// This could involve inserting COPYs before (for uses) or after (for defs).
/// This requires the number of operands to match the instruction description.
/// \returns whether operand regclass constraining succeeded.
///
// FIXME: Not all instructions have the same number of operands. We should
// probably expose a constrain helper per operand and let the target selector
// constrain individual registers, like fast-isel.
bool constrainSelectedInstRegOperands(MachineInstr &I,
                                      const TargetInstrInfo &TII,
                                      const TargetRegisterInfo &TRI,
                                      const RegisterBankInfo &RBI);
/// Check whether an instruction \p MI is dead: it only defines dead virtual
/// registers, and doesn't have other side effects.
bool isTriviallyDead(const MachineInstr &MI, const MachineRegisterInfo &MRI);

/// Report an ISel error as a missed optimization remark to the LLVMContext's
/// diagnostic stream.  Set the FailedISel MachineFunction property.
void reportGISelFailure(MachineFunction &MF, const TargetPassConfig &TPC,
                        MachineOptimizationRemarkEmitter &MORE,
                        MachineOptimizationRemarkMissed &R);

void reportGISelFailure(MachineFunction &MF, const TargetPassConfig &TPC,
                        MachineOptimizationRemarkEmitter &MORE,
                        const char *PassName, StringRef Msg,
                        const MachineInstr &MI);

/// If \p VReg is defined by a G_CONSTANT fits in int64_t
/// returns it.
Optional<int64_t> getConstantVRegVal(unsigned VReg,
                                     const MachineRegisterInfo &MRI);
/// Simple struct used to hold a constant integer value and a virtual
/// register.
struct ValueAndVReg {
  int64_t Value;
  unsigned VReg;
};
/// If \p VReg is defined by a statically evaluable chain of
/// instructions rooted on a G_CONSTANT (\p LookThroughInstrs == true)
/// and that constant fits in int64_t, returns its value as well as
/// the virtual register defined by this G_CONSTANT.
/// When \p LookThroughInstrs == false, this function behaves like
/// getConstantVRegVal.
Optional<ValueAndVReg>
getConstantVRegValWithLookThrough(unsigned VReg, const MachineRegisterInfo &MRI,
                                  bool LookThroughInstrs = true);
const ConstantFP* getConstantFPVRegVal(unsigned VReg,
                                       const MachineRegisterInfo &MRI);

/// See if Reg is defined by an single def instruction that is
/// Opcode. Also try to do trivial folding if it's a COPY with
/// same types. Returns null otherwise.
MachineInstr *getOpcodeDef(unsigned Opcode, unsigned Reg,
                           const MachineRegisterInfo &MRI);

/// Returns an APFloat from Val converted to the appropriate size.
APFloat getAPFloatFromSize(double Val, unsigned Size);

/// Modify analysis usage so it preserves passes required for the SelectionDAG
/// fallback.
void getSelectionDAGFallbackAnalysisUsage(AnalysisUsage &AU);

Optional<APInt> ConstantFoldBinOp(unsigned Opcode, const unsigned Op1,
                                  const unsigned Op2,
                                  const MachineRegisterInfo &MRI);
} // End namespace llvm.
#endif
