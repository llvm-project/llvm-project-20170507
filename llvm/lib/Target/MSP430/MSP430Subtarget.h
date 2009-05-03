//====-- MSP430Subtarget.h - Define Subtarget for the MSP430 ---*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MSP430 specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MSP430_SUBTARGET_H
#define LLVM_TARGET_MSP430_SUBTARGET_H

#include "llvm/Target/TargetSubtarget.h"

#include <string>

namespace llvm {
class Module;
class TargetMachine;

class MSP430Subtarget : public TargetSubtarget {
  bool ExtendedInsts;
public:
  /// This constructor initializes the data members to match that
  /// of the specified module.
  ///
  MSP430Subtarget(const TargetMachine &TM, const Module &M,
                  const std::string &FS);

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(const std::string &FS, const std::string &CPU);
};
} // End llvm namespace

#endif  // LLVM_TARGET_MSP430_SUBTARGET_H
