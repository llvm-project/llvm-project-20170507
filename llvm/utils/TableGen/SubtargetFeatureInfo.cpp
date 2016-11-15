//===- SubtargetFeatureInfo.cpp - Helpers for subtarget features ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SubtargetFeatureInfo.h"

#include "llvm/TableGen/Record.h"

#include <map>

using namespace llvm;

void SubtargetFeatureInfo::dump() const {
  errs() << getEnumName() << " " << Index << "\n";
  TheDef->dump();
}

std::vector<std::pair<Record *, SubtargetFeatureInfo>>
SubtargetFeatureInfo::getAll(const RecordKeeper &Records) {
  std::vector<std::pair<Record *, SubtargetFeatureInfo>> SubtargetFeatures;
  std::vector<Record *> AllPredicates =
      Records.getAllDerivedDefinitions("Predicate");
  for (Record *Pred : AllPredicates) {
    // Ignore predicates that are not intended for the assembler.
    //
    // The "AssemblerMatcherPredicate" string should be promoted to an argument
    // if we re-use the machinery for non-assembler purposes in future.
    if (!Pred->getValueAsBit("AssemblerMatcherPredicate"))
      continue;

    if (Pred->getName().empty())
      PrintFatalError(Pred->getLoc(), "Predicate has no name!");

    SubtargetFeatures.emplace_back(
        Pred, SubtargetFeatureInfo(Pred, SubtargetFeatures.size()));
  }
  return SubtargetFeatures;
}

void SubtargetFeatureInfo::emitComputeAvailableFeatures(
    StringRef TargetName, StringRef ClassName,
    std::map<Record *, SubtargetFeatureInfo, LessRecordByID> &SubtargetFeatures,
    raw_ostream &OS) {
  OS << "uint64_t " << TargetName << ClassName << "::\n"
     << "ComputeAvailableFeatures(const FeatureBitset& FB) const {\n";
  OS << "  uint64_t Features = 0;\n";
  for (const auto &SF : SubtargetFeatures) {
    const SubtargetFeatureInfo &SFI = SF.second;

    OS << "  if (";
    std::string CondStorage =
        SFI.TheDef->getValueAsString("AssemblerCondString");
    StringRef Conds = CondStorage;
    std::pair<StringRef, StringRef> Comma = Conds.split(',');
    bool First = true;
    do {
      if (!First)
        OS << " && ";

      bool Neg = false;
      StringRef Cond = Comma.first;
      if (Cond[0] == '!') {
        Neg = true;
        Cond = Cond.substr(1);
      }

      OS << "(";
      if (Neg)
        OS << "!";
      OS << "FB[" << TargetName << "::" << Cond << "])";

      if (Comma.second.empty())
        break;

      First = false;
      Comma = Comma.second.split(',');
    } while (true);

    OS << ")\n";
    OS << "    Features |= " << SFI.getEnumName() << ";\n";
  }
  OS << "  return Features;\n";
  OS << "}\n\n";
}
