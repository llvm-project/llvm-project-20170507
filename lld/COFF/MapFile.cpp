//===- MapFile.cpp --------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the /lldmap option. It shows lists in order and
// hierarchically the output sections, input sections, input files and
// symbol:
//
//   Address  Size     Align Out     File    Symbol
//   00201000 00000015     4 .text
//   00201000 0000000e     4         test.o:(.text)
//   0020100e 00000000     0                 local
//   00201005 00000000     0                 f(int)
//
//===----------------------------------------------------------------------===//

#include "MapFile.h"
#include "SymbolTable.h"
#include "Symbols.h"
#include "Writer.h"
#include "lld/Common/ErrorHandler.h"
#include "lld/Common/Threads.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::object;

using namespace lld;
using namespace lld::coff;

using SymbolMapTy =
    DenseMap<const SectionChunk *, SmallVector<DefinedRegular *, 4>>;

static const std::string indent8 = "        ";          // 8 spaces
static const std::string indent16 = "                "; // 16 spaces

// Print out the first three columns of a line.
static void writeHeader(raw_ostream &os, uint64_t addr, uint64_t size,
                        uint64_t align) {
  os << format("%08llx %08llx %5lld ", addr, size, align);
}

// Returns a list of all symbols that we want to print out.
static std::vector<DefinedRegular *> getSymbols() {
  std::vector<DefinedRegular *> v;
  for (ObjFile *file : ObjFile::instances)
    for (Symbol *b : file->getSymbols())
      if (auto *sym = dyn_cast_or_null<DefinedRegular>(b))
        if (sym && !sym->getCOFFSymbol().isSectionDefinition())
          v.push_back(sym);
  return v;
}

// Returns a map from sections to their symbols.
static SymbolMapTy getSectionSyms(ArrayRef<DefinedRegular *> syms) {
  SymbolMapTy ret;
  for (DefinedRegular *s : syms)
    ret[s->getChunk()].push_back(s);

  // Sort symbols by address.
  for (auto &it : ret) {
    SmallVectorImpl<DefinedRegular *> &v = it.second;
    std::sort(v.begin(), v.end(), [](DefinedRegular *a, DefinedRegular *b) {
      return a->getRVA() < b->getRVA();
    });
  }
  return ret;
}

// Construct a map from symbols to their stringified representations.
static DenseMap<DefinedRegular *, std::string>
getSymbolStrings(ArrayRef<DefinedRegular *> syms) {
  std::vector<std::string> str(syms.size());
  parallelForEachN((size_t)0, syms.size(), [&](size_t i) {
    raw_string_ostream os(str[i]);
    writeHeader(os, syms[i]->getRVA(), 0, 0);
    os << indent16 << toString(*syms[i]);
  });

  DenseMap<DefinedRegular *, std::string> ret;
  for (size_t i = 0, e = syms.size(); i < e; ++i)
    ret[syms[i]] = std::move(str[i]);
  return ret;
}

void coff::writeMapFile(ArrayRef<OutputSection *> outputSections) {
  if (config->mapFile.empty())
    return;

  std::error_code ec;
  raw_fd_ostream os(config->mapFile, ec, sys::fs::F_None);
  if (ec)
    fatal("cannot open " + config->mapFile + ": " + ec.message());

  // Collect symbol info that we want to print out.
  std::vector<DefinedRegular *> syms = getSymbols();
  SymbolMapTy sectionSyms = getSectionSyms(syms);
  DenseMap<DefinedRegular *, std::string> symStr = getSymbolStrings(syms);

  // Print out the header line.
  os << "Address  Size     Align Out     In      Symbol\n";

  // Print out file contents.
  for (OutputSection *sec : outputSections) {
    writeHeader(os, sec->getRVA(), sec->getVirtualSize(), /*Align=*/pageSize);
    os << sec->name << '\n';

    for (Chunk *c : sec->chunks) {
      auto *sc = dyn_cast<SectionChunk>(c);
      if (!sc)
        continue;

      writeHeader(os, sc->getRVA(), sc->getSize(), sc->getAlignment());
      os << indent8 << sc->file->getName() << ":(" << sc->getSectionName()
         << ")\n";
      for (DefinedRegular *sym : sectionSyms[sc])
        os << symStr[sym] << '\n';
    }
  }
}
