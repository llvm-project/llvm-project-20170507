//===-- Mips/MipsCodeEmitter.cpp - Convert Mips code to machine code -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//
//
// This file contains the pass that transforms the Mips machine instructions
// into relocatable machine code.
//
//===---------------------------------------------------------------------===//

#define DEBUG_TYPE "jit"
#include "Mips.h"
#include "MipsInstrInfo.h"
#include "MipsRelocations.h"
#include "MipsSubtarget.h"
#include "MipsTargetMachine.h"
#include "MCTargetDesc/MipsBaseInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/JITCodeEmitter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#ifndef NDEBUG
#include <iomanip>
#endif

using namespace llvm;

STATISTIC(NumEmitted, "Number of machine instructions emitted");

namespace {

class MipsCodeEmitter : public MachineFunctionPass {
  MipsJITInfo *JTI;
  const MipsInstrInfo *II;
  const TargetData *TD;
  const MipsSubtarget *Subtarget;
  TargetMachine &TM;
  JITCodeEmitter &MCE;
  const std::vector<MachineConstantPoolEntry> *MCPEs;
  const std::vector<MachineJumpTableEntry> *MJTEs;
  bool IsPIC;

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<MachineModuleInfo> ();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  static char ID;

  public:
    MipsCodeEmitter(TargetMachine &tm, JITCodeEmitter &mce) :
      MachineFunctionPass(ID), JTI(0),
      II((const MipsInstrInfo *) tm.getInstrInfo()),
      TD(tm.getTargetData()), TM(tm), MCE(mce), MCPEs(0), MJTEs(0),
      IsPIC(TM.getRelocationModel() == Reloc::PIC_) {
    }

    bool runOnMachineFunction(MachineFunction &MF);

    virtual const char *getPassName() const {
      return "Mips Machine Code Emitter";
    }

    /// getBinaryCodeForInstr - This function, generated by the
    /// CodeEmitterGenerator using TableGen, produces the binary encoding for
    /// machine instructions.
    unsigned getBinaryCodeForInstr(const MachineInstr &MI) const;

    void emitInstruction(const MachineInstr &MI);

  private:

    void emitWordLE(unsigned Word);

    /// Routines that handle operands which add machine relocations which are
    /// fixed up by the relocation stage.
    void emitGlobalAddress(const GlobalValue *GV, unsigned Reloc,
                           bool MayNeedFarStub) const;
    void emitExternalSymbolAddress(const char *ES, unsigned Reloc) const;
    void emitConstPoolAddress(unsigned CPI, unsigned Reloc) const;
    void emitJumpTableAddress(unsigned JTIndex, unsigned Reloc) const;
    void emitMachineBasicBlock(MachineBasicBlock *BB, unsigned Reloc) const;

    /// getMachineOpValue - Return binary encoding of operand. If the machine
    /// operand requires relocation, record the relocation and return zero.
    unsigned getMachineOpValue(const MachineInstr &MI,
                               const MachineOperand &MO) const;

    unsigned getRelocation(const MachineInstr &MI,
                           const MachineOperand &MO) const;

    unsigned getJumpTargetOpValue(const MachineInstr &MI, unsigned OpNo) const;

    unsigned getBranchTargetOpValue(const MachineInstr &MI,
                                    unsigned OpNo) const;
    unsigned getMemEncoding(const MachineInstr &MI, unsigned OpNo) const;
    unsigned getSizeExtEncoding(const MachineInstr &MI, unsigned OpNo) const;
    unsigned getSizeInsEncoding(const MachineInstr &MI, unsigned OpNo) const;

    int emitULW(const MachineInstr &MI);
    int emitUSW(const MachineInstr &MI);
    int emitULH(const MachineInstr &MI);
    int emitULHu(const MachineInstr &MI);
    int emitUSH(const MachineInstr &MI);

    void emitGlobalAddressUnaligned(const GlobalValue *GV, unsigned Reloc,
                                    int Offset) const;
  };
}

char MipsCodeEmitter::ID = 0;

bool MipsCodeEmitter::runOnMachineFunction(MachineFunction &MF) {
  JTI = ((MipsTargetMachine&) MF.getTarget()).getJITInfo();
  II = ((const MipsTargetMachine&) MF.getTarget()).getInstrInfo();
  TD = ((const MipsTargetMachine&) MF.getTarget()).getTargetData();
  Subtarget = &TM.getSubtarget<MipsSubtarget> ();
  MCPEs = &MF.getConstantPool()->getConstants();
  MJTEs = 0;
  if (MF.getJumpTableInfo()) MJTEs = &MF.getJumpTableInfo()->getJumpTables();
  JTI->Initialize(MF, IsPIC);
  MCE.setModuleInfo(&getAnalysis<MachineModuleInfo> ());

  do {
    DEBUG(errs() << "JITTing function '"
        << MF.getFunction()->getName() << "'\n");
    MCE.startFunction(MF);

    for (MachineFunction::iterator MBB = MF.begin(), E = MF.end();
        MBB != E; ++MBB){
      MCE.StartMachineBasicBlock(MBB);
      for (MachineBasicBlock::iterator I = MBB->begin(), E = MBB->end();
          I != E; ++I)
        emitInstruction(*I);
    }
  } while (MCE.finishFunction(MF));

  return false;
}

unsigned MipsCodeEmitter::getRelocation(const MachineInstr &MI,
                                        const MachineOperand &MO) const {
  // NOTE: This relocations are for static.
  uint64_t TSFlags = MI.getDesc().TSFlags;
  uint64_t Form = TSFlags & MipsII::FormMask;
  if (Form == MipsII::FrmJ)
    return Mips::reloc_mips_26;
  if ((Form == MipsII::FrmI || Form == MipsII::FrmFI)
       && MI.isBranch())
    return Mips::reloc_mips_branch;
  if (Form == MipsII::FrmI && MI.getOpcode() == Mips::LUi)
    return Mips::reloc_mips_hi;
  return Mips::reloc_mips_lo;
}

unsigned MipsCodeEmitter::getJumpTargetOpValue(const MachineInstr &MI,
                                               unsigned OpNo) const {
  // FIXME: implement
  return 0;
}

unsigned MipsCodeEmitter::getBranchTargetOpValue(const MachineInstr &MI,
                                                 unsigned OpNo) const {
  // FIXME: implement
  return 0;
}

unsigned MipsCodeEmitter::getMemEncoding(const MachineInstr &MI,
                                         unsigned OpNo) const {
  // Base register is encoded in bits 20-16, offset is encoded in bits 15-0.
  assert(MI.getOperand(OpNo).isReg());
  unsigned RegBits = getMachineOpValue(MI, MI.getOperand(OpNo)) << 16;
  return (getMachineOpValue(MI, MI.getOperand(OpNo+1)) & 0xFFFF) | RegBits;
}

unsigned MipsCodeEmitter::getSizeExtEncoding(const MachineInstr &MI,
                                             unsigned OpNo) const {
  // size is encoded as size-1.
  return getMachineOpValue(MI, MI.getOperand(OpNo)) - 1;
}

unsigned MipsCodeEmitter::getSizeInsEncoding(const MachineInstr &MI,
                                             unsigned OpNo) const {
  // size is encoded as pos+size-1.
  return getMachineOpValue(MI, MI.getOperand(OpNo-1)) +
         getMachineOpValue(MI, MI.getOperand(OpNo)) - 1;
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned MipsCodeEmitter::getMachineOpValue(const MachineInstr &MI,
                                            const MachineOperand &MO) const {
  if (MO.isReg())
    return MipsRegisterInfo::getRegisterNumbering(MO.getReg());
  else if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());
  else if (MO.isGlobal()) {
    if (MI.getOpcode() == Mips::ULW || MI.getOpcode() == Mips::USW ||
          MI.getOpcode() == Mips::ULH || MI.getOpcode() == Mips::ULHu)
      emitGlobalAddressUnaligned(MO.getGlobal(), getRelocation(MI, MO), 4);
    else if (MI.getOpcode() == Mips::USH)
      emitGlobalAddressUnaligned(MO.getGlobal(), getRelocation(MI, MO), 8);
    else
      emitGlobalAddress(MO.getGlobal(), getRelocation(MI, MO), true);
  } else if (MO.isSymbol())
    emitExternalSymbolAddress(MO.getSymbolName(), getRelocation(MI, MO));
  else if (MO.isCPI())
    emitConstPoolAddress(MO.getIndex(), getRelocation(MI, MO));
  else if (MO.isJTI())
    emitJumpTableAddress(MO.getIndex(), getRelocation(MI, MO));
  else if (MO.isMBB())
    emitMachineBasicBlock(MO.getMBB(), getRelocation(MI, MO));
  else
    llvm_unreachable("Unable to encode MachineOperand!");
  return 0;
}

void MipsCodeEmitter::emitGlobalAddress(const GlobalValue *GV, unsigned Reloc,
                                        bool MayNeedFarStub) const {
  MCE.addRelocation(MachineRelocation::getGV(MCE.getCurrentPCOffset(), Reloc,
                                             const_cast<GlobalValue *>(GV), 0,
                                             MayNeedFarStub));
}

void MipsCodeEmitter::emitGlobalAddressUnaligned(const GlobalValue *GV,
                                           unsigned Reloc, int Offset) const {
  MCE.addRelocation(MachineRelocation::getGV(MCE.getCurrentPCOffset(), Reloc,
                             const_cast<GlobalValue *>(GV), 0, false));
  MCE.addRelocation(MachineRelocation::getGV(MCE.getCurrentPCOffset() + Offset,
                      Reloc, const_cast<GlobalValue *>(GV), 0, false));
}

void MipsCodeEmitter::
emitExternalSymbolAddress(const char *ES, unsigned Reloc) const {
  MCE.addRelocation(MachineRelocation::getExtSym(MCE.getCurrentPCOffset(),
                                                 Reloc, ES, 0, 0, false));
}

void MipsCodeEmitter::emitConstPoolAddress(unsigned CPI, unsigned Reloc) const {
  MCE.addRelocation(MachineRelocation::getConstPool(MCE.getCurrentPCOffset(),
                                                    Reloc, CPI, 0, false));
}

void MipsCodeEmitter::
emitJumpTableAddress(unsigned JTIndex, unsigned Reloc) const {
  MCE.addRelocation(MachineRelocation::getJumpTable(MCE.getCurrentPCOffset(),
                                                    Reloc, JTIndex, 0, false));
}

void MipsCodeEmitter::emitMachineBasicBlock(MachineBasicBlock *BB,
                                            unsigned Reloc) const {
  MCE.addRelocation(MachineRelocation::getBB(MCE.getCurrentPCOffset(),
                                             Reloc, BB));
}

int MipsCodeEmitter::emitUSW(const MachineInstr &MI) {
  unsigned src = getMachineOpValue(MI, MI.getOperand(0));
  unsigned base = getMachineOpValue(MI, MI.getOperand(1));
  unsigned offset = getMachineOpValue(MI, MI.getOperand(2));
  // swr src, offset(base)
  // swl src, offset+3(base)
  MCE.emitWordLE(
    (0x2e << 26) | (base << 21) | (src << 16) | (offset & 0xffff));
  MCE.emitWordLE(
    (0x2a << 26) | (base << 21) | (src << 16) | ((offset+3) & 0xffff));
  return 2;
}

int MipsCodeEmitter::emitULW(const MachineInstr &MI) {
  unsigned dst = getMachineOpValue(MI, MI.getOperand(0));
  unsigned base = getMachineOpValue(MI, MI.getOperand(1));
  unsigned offset = getMachineOpValue(MI, MI.getOperand(2));
  unsigned at = 1;
  if (dst != base) {
    // lwr dst, offset(base)
    // lwl dst, offset+3(base)
    MCE.emitWordLE(
      (0x26 << 26) | (base << 21) | (dst << 16) | (offset & 0xffff));
    MCE.emitWordLE(
      (0x22 << 26) | (base << 21) | (dst << 16) | ((offset+3) & 0xffff));
    return 2;
  } else {
    // lwr at, offset(base)
    // lwl at, offset+3(base)
    // addu dst, at, $zero
    MCE.emitWordLE(
      (0x26 << 26) | (base << 21) | (at << 16) | (offset & 0xffff));
    MCE.emitWordLE(
      (0x22 << 26) | (base << 21) | (at << 16) | ((offset+3) & 0xffff));
    MCE.emitWordLE(
      (0x0 << 26) | (at << 21) | (0x0 << 16) | (dst << 11) | (0x0 << 6) | 0x21);
    return 3;
  }
}

int MipsCodeEmitter::emitUSH(const MachineInstr &MI) {
  unsigned src = getMachineOpValue(MI, MI.getOperand(0));
  unsigned base = getMachineOpValue(MI, MI.getOperand(1));
  unsigned offset = getMachineOpValue(MI, MI.getOperand(2));
  unsigned at = 1;
  // sb src, offset(base)
  // srl at,src,8
  // sb at, offset+1(base)
  MCE.emitWordLE(
    (0x28 << 26) | (base << 21) | (src << 16) | (offset & 0xffff));
  MCE.emitWordLE(
    (0x0 << 26) | (0x0 << 21) | (src << 16) | (at << 11) | (0x8 << 6) | 0x2);
  MCE.emitWordLE(
    (0x28 << 26) | (base << 21) | (at << 16) | ((offset+1) & 0xffff));
  return 3;
}

int MipsCodeEmitter::emitULH(const MachineInstr &MI) {
  unsigned dst = getMachineOpValue(MI, MI.getOperand(0));
  unsigned base = getMachineOpValue(MI, MI.getOperand(1));
  unsigned offset = getMachineOpValue(MI, MI.getOperand(2));
  unsigned at = 1;
  // lbu at, offset(base)
  // lb dst, offset+1(base)
  // sll dst,dst,8
  // or dst,dst,at
  MCE.emitWordLE(
    (0x24 << 26) | (base << 21) | (at << 16) | (offset & 0xffff));
  MCE.emitWordLE(
    (0x20 << 26) | (base << 21) | (dst << 16) | ((offset+1) & 0xffff));
  MCE.emitWordLE(
    (0x0 << 26) | (0x0 << 21) | (dst << 16) | (dst << 11) | (0x8 << 6) | 0x0);
  MCE.emitWordLE(
    (0x0 << 26) | (dst << 21) | (at << 16) | (dst << 11) | (0x0 << 6) | 0x25);
  return 4;
}

int MipsCodeEmitter::emitULHu(const MachineInstr &MI) {
  unsigned dst = getMachineOpValue(MI, MI.getOperand(0));
  unsigned base = getMachineOpValue(MI, MI.getOperand(1));
  unsigned offset = getMachineOpValue(MI, MI.getOperand(2));
  unsigned at = 1;
  // lbu at, offset(base)
  // lbu dst, offset+1(base)
  // sll dst,dst,8
  // or dst,dst,at
  MCE.emitWordLE(
    (0x24 << 26) | (base << 21) | (at << 16) | (offset & 0xffff));
  MCE.emitWordLE(
    (0x24 << 26) | (base << 21) | (dst << 16) | ((offset+1) & 0xffff));
  MCE.emitWordLE(
    (0x0 << 26) | (0x0 << 21) | (dst << 16) | (dst << 11) | (0x8 << 6) | 0x0);
  MCE.emitWordLE(
    (0x0 << 26) | (dst << 21) | (at << 16) | (dst << 11) | (0x0 << 6) | 0x25);
  return 4;
}

void MipsCodeEmitter::emitInstruction(const MachineInstr &MI) {
  DEBUG(errs() << "JIT: " << (void*)MCE.getCurrentPCValue() << ":\t" << MI);

  MCE.processDebugLoc(MI.getDebugLoc(), true);

  // Skip pseudo instructions.
  if ((MI.getDesc().TSFlags & MipsII::FormMask) == MipsII::Pseudo)
    return;


  switch (MI.getOpcode()) {
  case Mips::USW:
    NumEmitted += emitUSW(MI);
    break;
  case Mips::ULW:
    NumEmitted += emitULW(MI);
    break;
  case Mips::ULH:
    NumEmitted += emitULH(MI);
    break;
  case Mips::ULHu:
    NumEmitted += emitULHu(MI);
    break;
  case Mips::USH:
    NumEmitted += emitUSH(MI);
    break;

  default:
    emitWordLE(getBinaryCodeForInstr(MI));
    ++NumEmitted;  // Keep track of the # of mi's emitted
    break;
  }

  MCE.processDebugLoc(MI.getDebugLoc(), false);
}

void MipsCodeEmitter::emitWordLE(unsigned Word) {
  DEBUG(errs() << "  0x";
        errs().write_hex(Word) << "\n");
  MCE.emitWordLE(Word);
}

/// createMipsJITCodeEmitterPass - Return a pass that emits the collected Mips
/// code to the specified MCE object.
FunctionPass *llvm::createMipsJITCodeEmitterPass(MipsTargetMachine &TM,
                                                 JITCodeEmitter &JCE) {
  return new MipsCodeEmitter(TM, JCE);
}

#include "MipsGenCodeEmitter.inc"
