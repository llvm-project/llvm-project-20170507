//===- DLL.cpp ------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines various types of chunks for the DLL import or export
// descriptor tables. They are inherently Windows-specific.
// You need to read Microsoft PE/COFF spec to understand details
// about the data structures.
//
// If you are not particularly interested in linking against Windows
// DLL, you can skip this file, and you should still be able to
// understand the rest of the linker.
//
//===----------------------------------------------------------------------===//

#include "DLL.h"
#include "Chunks.h"
#include "llvm/Object/COFF.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Path.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::COFF;

namespace lld {
namespace coff {
namespace {

// Import table

// A chunk for the import descriptor table.
class HintNameChunk : public NonSectionChunk {
public:
  HintNameChunk(StringRef n, uint16_t h) : name(n), hint(h) {}

  size_t getSize() const override {
    // Starts with 2 byte Hint field, followed by a null-terminated string,
    // ends with 0 or 1 byte padding.
    return alignTo(name.size() + 3, 2);
  }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, getSize());
    write16le(buf, hint);
    memcpy(buf + 2, name.data(), name.size());
  }

private:
  StringRef name;
  uint16_t hint;
};

// A chunk for the import descriptor table.
class LookupChunk : public NonSectionChunk {
public:
  explicit LookupChunk(Chunk *c) : hintName(c) {
    setAlignment(config->wordsize);
  }
  size_t getSize() const override { return config->wordsize; }

  void writeTo(uint8_t *buf) const override {
    if (config->is64())
      write64le(buf, hintName->getRVA());
    else
      write32le(buf, hintName->getRVA());
  }

  Chunk *hintName;
};

// A chunk for the import descriptor table.
// This chunk represent import-by-ordinal symbols.
// See Microsoft PE/COFF spec 7.1. Import Header for details.
class OrdinalOnlyChunk : public NonSectionChunk {
public:
  explicit OrdinalOnlyChunk(uint16_t v) : ordinal(v) {
    setAlignment(config->wordsize);
  }
  size_t getSize() const override { return config->wordsize; }

  void writeTo(uint8_t *buf) const override {
    // An import-by-ordinal slot has MSB 1 to indicate that
    // this is import-by-ordinal (and not import-by-name).
    if (config->is64()) {
      write64le(buf, (1ULL << 63) | ordinal);
    } else {
      write32le(buf, (1ULL << 31) | ordinal);
    }
  }

  uint16_t ordinal;
};

// A chunk for the import descriptor table.
class ImportDirectoryChunk : public NonSectionChunk {
public:
  explicit ImportDirectoryChunk(Chunk *n) : dllName(n) {}
  size_t getSize() const override { return sizeof(ImportDirectoryTableEntry); }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, getSize());

    auto *e = (coff_import_directory_table_entry *)(buf);
    e->ImportLookupTableRVA = lookupTab->getRVA();
    e->NameRVA = dllName->getRVA();
    e->ImportAddressTableRVA = addressTab->getRVA();
  }

  Chunk *dllName;
  Chunk *lookupTab;
  Chunk *addressTab;
};

// A chunk representing null terminator in the import table.
// Contents of this chunk is always null bytes.
class NullChunk : public NonSectionChunk {
public:
  explicit NullChunk(size_t n) : size(n) { hasData = false; }
  size_t getSize() const override { return size; }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, size);
  }

private:
  size_t size;
};

static std::vector<std::vector<DefinedImportData *>>
binImports(const std::vector<DefinedImportData *> &imports) {
  // Group DLL-imported symbols by DLL name because that's how
  // symbols are layed out in the import descriptor table.
  auto less = [](const std::string &a, const std::string &b) {
    return config->dllOrder[a] < config->dllOrder[b];
  };
  std::map<std::string, std::vector<DefinedImportData *>,
           bool(*)(const std::string &, const std::string &)> m(less);
  for (DefinedImportData *sym : imports)
    m[sym->getDLLName().lower()].push_back(sym);

  std::vector<std::vector<DefinedImportData *>> v;
  for (auto &kv : m) {
    // Sort symbols by name for each group.
    std::vector<DefinedImportData *> &syms = kv.second;
    std::sort(syms.begin(), syms.end(),
              [](DefinedImportData *a, DefinedImportData *b) {
                return a->getName() < b->getName();
              });
    v.push_back(std::move(syms));
  }
  return v;
}

// Export table
// See Microsoft PE/COFF spec 4.3 for details.

// A chunk for the delay import descriptor table etnry.
class DelayDirectoryChunk : public NonSectionChunk {
public:
  explicit DelayDirectoryChunk(Chunk *n) : dllName(n) {}

  size_t getSize() const override {
    return sizeof(delay_import_directory_table_entry);
  }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, getSize());

    auto *e = (delay_import_directory_table_entry *)(buf);
    e->Attributes = 1;
    e->Name = dllName->getRVA();
    e->ModuleHandle = moduleHandle->getRVA();
    e->DelayImportAddressTable = addressTab->getRVA();
    e->DelayImportNameTable = nameTab->getRVA();
  }

  Chunk *dllName;
  Chunk *moduleHandle;
  Chunk *addressTab;
  Chunk *nameTab;
};

// Initial contents for delay-loaded functions.
// This code calls __delayLoadHelper2 function to resolve a symbol
// and then overwrites its jump table slot with the result
// for subsequent function calls.
static const uint8_t thunkX64[] = {
    0x51,                               // push    rcx
    0x52,                               // push    rdx
    0x41, 0x50,                         // push    r8
    0x41, 0x51,                         // push    r9
    0x48, 0x83, 0xEC, 0x48,             // sub     rsp, 48h
    0x66, 0x0F, 0x7F, 0x04, 0x24,       // movdqa  xmmword ptr [rsp], xmm0
    0x66, 0x0F, 0x7F, 0x4C, 0x24, 0x10, // movdqa  xmmword ptr [rsp+10h], xmm1
    0x66, 0x0F, 0x7F, 0x54, 0x24, 0x20, // movdqa  xmmword ptr [rsp+20h], xmm2
    0x66, 0x0F, 0x7F, 0x5C, 0x24, 0x30, // movdqa  xmmword ptr [rsp+30h], xmm3
    0x48, 0x8D, 0x15, 0, 0, 0, 0,       // lea     rdx, [__imp_<FUNCNAME>]
    0x48, 0x8D, 0x0D, 0, 0, 0, 0,       // lea     rcx, [___DELAY_IMPORT_...]
    0xE8, 0, 0, 0, 0,                   // call    __delayLoadHelper2
    0x66, 0x0F, 0x6F, 0x04, 0x24,       // movdqa  xmm0, xmmword ptr [rsp]
    0x66, 0x0F, 0x6F, 0x4C, 0x24, 0x10, // movdqa  xmm1, xmmword ptr [rsp+10h]
    0x66, 0x0F, 0x6F, 0x54, 0x24, 0x20, // movdqa  xmm2, xmmword ptr [rsp+20h]
    0x66, 0x0F, 0x6F, 0x5C, 0x24, 0x30, // movdqa  xmm3, xmmword ptr [rsp+30h]
    0x48, 0x83, 0xC4, 0x48,             // add     rsp, 48h
    0x41, 0x59,                         // pop     r9
    0x41, 0x58,                         // pop     r8
    0x5A,                               // pop     rdx
    0x59,                               // pop     rcx
    0xFF, 0xE0,                         // jmp     rax
};

static const uint8_t thunkX86[] = {
    0x51,              // push  ecx
    0x52,              // push  edx
    0x68, 0, 0, 0, 0,  // push  offset ___imp__<FUNCNAME>
    0x68, 0, 0, 0, 0,  // push  offset ___DELAY_IMPORT_DESCRIPTOR_<DLLNAME>_dll
    0xE8, 0, 0, 0, 0,  // call  ___delayLoadHelper2@8
    0x5A,              // pop   edx
    0x59,              // pop   ecx
    0xFF, 0xE0,        // jmp   eax
};

static const uint8_t thunkARM[] = {
    0x40, 0xf2, 0x00, 0x0c, // mov.w   ip, #0 __imp_<FUNCNAME>
    0xc0, 0xf2, 0x00, 0x0c, // mov.t   ip, #0 __imp_<FUNCNAME>
    0x2d, 0xe9, 0x0f, 0x48, // push.w  {r0, r1, r2, r3, r11, lr}
    0x0d, 0xf2, 0x10, 0x0b, // addw    r11, sp, #16
    0x2d, 0xed, 0x10, 0x0b, // vpush   {d0, d1, d2, d3, d4, d5, d6, d7}
    0x61, 0x46,             // mov     r1, ip
    0x40, 0xf2, 0x00, 0x00, // mov.w   r0, #0 DELAY_IMPORT_DESCRIPTOR
    0xc0, 0xf2, 0x00, 0x00, // mov.t   r0, #0 DELAY_IMPORT_DESCRIPTOR
    0x00, 0xf0, 0x00, 0xd0, // bl      #0 __delayLoadHelper2
    0x84, 0x46,             // mov     ip, r0
    0xbd, 0xec, 0x10, 0x0b, // vpop    {d0, d1, d2, d3, d4, d5, d6, d7}
    0xbd, 0xe8, 0x0f, 0x48, // pop.w   {r0, r1, r2, r3, r11, lr}
    0x60, 0x47,             // bx      ip
};

static const uint8_t thunkARM64[] = {
    0x11, 0x00, 0x00, 0x90, // adrp    x17, #0      __imp_<FUNCNAME>
    0x31, 0x02, 0x00, 0x91, // add     x17, x17, #0 :lo12:__imp_<FUNCNAME>
    0xfd, 0x7b, 0xb3, 0xa9, // stp     x29, x30, [sp, #-208]!
    0xfd, 0x03, 0x00, 0x91, // mov     x29, sp
    0xe0, 0x07, 0x01, 0xa9, // stp     x0, x1, [sp, #16]
    0xe2, 0x0f, 0x02, 0xa9, // stp     x2, x3, [sp, #32]
    0xe4, 0x17, 0x03, 0xa9, // stp     x4, x5, [sp, #48]
    0xe6, 0x1f, 0x04, 0xa9, // stp     x6, x7, [sp, #64]
    0xe0, 0x87, 0x02, 0xad, // stp     q0, q1, [sp, #80]
    0xe2, 0x8f, 0x03, 0xad, // stp     q2, q3, [sp, #112]
    0xe4, 0x97, 0x04, 0xad, // stp     q4, q5, [sp, #144]
    0xe6, 0x9f, 0x05, 0xad, // stp     q6, q7, [sp, #176]
    0xe1, 0x03, 0x11, 0xaa, // mov     x1, x17
    0x00, 0x00, 0x00, 0x90, // adrp    x0, #0     DELAY_IMPORT_DESCRIPTOR
    0x00, 0x00, 0x00, 0x91, // add     x0, x0, #0 :lo12:DELAY_IMPORT_DESCRIPTOR
    0x00, 0x00, 0x00, 0x94, // bl      #0 __delayLoadHelper2
    0xf0, 0x03, 0x00, 0xaa, // mov     x16, x0
    0xe6, 0x9f, 0x45, 0xad, // ldp     q6, q7, [sp, #176]
    0xe4, 0x97, 0x44, 0xad, // ldp     q4, q5, [sp, #144]
    0xe2, 0x8f, 0x43, 0xad, // ldp     q2, q3, [sp, #112]
    0xe0, 0x87, 0x42, 0xad, // ldp     q0, q1, [sp, #80]
    0xe6, 0x1f, 0x44, 0xa9, // ldp     x6, x7, [sp, #64]
    0xe4, 0x17, 0x43, 0xa9, // ldp     x4, x5, [sp, #48]
    0xe2, 0x0f, 0x42, 0xa9, // ldp     x2, x3, [sp, #32]
    0xe0, 0x07, 0x41, 0xa9, // ldp     x0, x1, [sp, #16]
    0xfd, 0x7b, 0xcd, 0xa8, // ldp     x29, x30, [sp], #208
    0x00, 0x02, 0x1f, 0xd6, // br      x16
};

// A chunk for the delay import thunk.
class ThunkChunkX64 : public NonSectionChunk {
public:
  ThunkChunkX64(Defined *i, Chunk *d, Defined *h)
      : imp(i), desc(d), helper(h) {}

  size_t getSize() const override { return sizeof(thunkX64); }

  void writeTo(uint8_t *buf) const override {
    memcpy(buf, thunkX64, sizeof(thunkX64));
    write32le(buf + 36, imp->getRVA() - rva - 40);
    write32le(buf + 43, desc->getRVA() - rva - 47);
    write32le(buf + 48, helper->getRVA() - rva - 52);
  }

  Defined *imp = nullptr;
  Chunk *desc = nullptr;
  Defined *helper = nullptr;
};

class ThunkChunkX86 : public NonSectionChunk {
public:
  ThunkChunkX86(Defined *i, Chunk *d, Defined *h)
      : imp(i), desc(d), helper(h) {}

  size_t getSize() const override { return sizeof(thunkX86); }

  void writeTo(uint8_t *buf) const override {
    memcpy(buf, thunkX86, sizeof(thunkX86));
    write32le(buf + 3, imp->getRVA() + config->imageBase);
    write32le(buf + 8, desc->getRVA() + config->imageBase);
    write32le(buf + 13, helper->getRVA() - rva - 17);
  }

  void getBaserels(std::vector<Baserel> *res) override {
    res->emplace_back(rva + 3);
    res->emplace_back(rva + 8);
  }

  Defined *imp = nullptr;
  Chunk *desc = nullptr;
  Defined *helper = nullptr;
};

class ThunkChunkARM : public NonSectionChunk {
public:
  ThunkChunkARM(Defined *i, Chunk *d, Defined *h)
      : imp(i), desc(d), helper(h) {}

  size_t getSize() const override { return sizeof(thunkARM); }

  void writeTo(uint8_t *buf) const override {
    memcpy(buf, thunkARM, sizeof(thunkARM));
    applyMOV32T(buf + 0, imp->getRVA() + config->imageBase);
    applyMOV32T(buf + 22, desc->getRVA() + config->imageBase);
    applyBranch24T(buf + 30, helper->getRVA() - rva - 34);
  }

  void getBaserels(std::vector<Baserel> *res) override {
    res->emplace_back(rva + 0, IMAGE_REL_BASED_ARM_MOV32T);
    res->emplace_back(rva + 22, IMAGE_REL_BASED_ARM_MOV32T);
  }

  Defined *imp = nullptr;
  Chunk *desc = nullptr;
  Defined *helper = nullptr;
};

class ThunkChunkARM64 : public NonSectionChunk {
public:
  ThunkChunkARM64(Defined *i, Chunk *d, Defined *h)
      : imp(i), desc(d), helper(h) {}

  size_t getSize() const override { return sizeof(thunkARM64); }

  void writeTo(uint8_t *buf) const override {
    memcpy(buf, thunkARM64, sizeof(thunkARM64));
    applyArm64Addr(buf + 0, imp->getRVA(), rva + 0, 12);
    applyArm64Imm(buf + 4, imp->getRVA() & 0xfff, 0);
    applyArm64Addr(buf + 52, desc->getRVA(), rva + 52, 12);
    applyArm64Imm(buf + 56, desc->getRVA() & 0xfff, 0);
    applyArm64Branch26(buf + 60, helper->getRVA() - rva - 60);
  }

  Defined *imp = nullptr;
  Chunk *desc = nullptr;
  Defined *helper = nullptr;
};

// A chunk for the import descriptor table.
class DelayAddressChunk : public NonSectionChunk {
public:
  explicit DelayAddressChunk(Chunk *c) : thunk(c) {
    setAlignment(config->wordsize);
  }
  size_t getSize() const override { return config->wordsize; }

  void writeTo(uint8_t *buf) const override {
    if (config->is64()) {
      write64le(buf, thunk->getRVA() + config->imageBase);
    } else {
      uint32_t bit = 0;
      // Pointer to thumb code must have the LSB set, so adjust it.
      if (config->machine == ARMNT)
        bit = 1;
      write32le(buf, (thunk->getRVA() + config->imageBase) | bit);
    }
  }

  void getBaserels(std::vector<Baserel> *res) override {
    res->emplace_back(rva);
  }

  Chunk *thunk;
};

// Export table
// Read Microsoft PE/COFF spec 5.3 for details.

// A chunk for the export descriptor table.
class ExportDirectoryChunk : public NonSectionChunk {
public:
  ExportDirectoryChunk(int i, int j, Chunk *d, Chunk *a, Chunk *n, Chunk *o)
      : maxOrdinal(i), nameTabSize(j), dllName(d), addressTab(a), nameTab(n),
        ordinalTab(o) {}

  size_t getSize() const override {
    return sizeof(export_directory_table_entry);
  }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, getSize());

    auto *e = (export_directory_table_entry *)(buf);
    e->NameRVA = dllName->getRVA();
    e->OrdinalBase = 0;
    e->AddressTableEntries = maxOrdinal + 1;
    e->NumberOfNamePointers = nameTabSize;
    e->ExportAddressTableRVA = addressTab->getRVA();
    e->NamePointerRVA = nameTab->getRVA();
    e->OrdinalTableRVA = ordinalTab->getRVA();
  }

  uint16_t maxOrdinal;
  uint16_t nameTabSize;
  Chunk *dllName;
  Chunk *addressTab;
  Chunk *nameTab;
  Chunk *ordinalTab;
};

class AddressTableChunk : public NonSectionChunk {
public:
  explicit AddressTableChunk(size_t maxOrdinal) : size(maxOrdinal + 1) {}
  size_t getSize() const override { return size * 4; }

  void writeTo(uint8_t *buf) const override {
    memset(buf, 0, getSize());

    for (const Export &e : config->exports) {
      uint8_t *p = buf + e.ordinal * 4;
      uint32_t bit = 0;
      // Pointer to thumb code must have the LSB set, so adjust it.
      if (config->machine == ARMNT && !e.data)
        bit = 1;
      if (e.forwardChunk) {
        write32le(p, e.forwardChunk->getRVA() | bit);
      } else {
        write32le(p, cast<Defined>(e.sym)->getRVA() | bit);
      }
    }
  }

private:
  size_t size;
};

class NamePointersChunk : public NonSectionChunk {
public:
  explicit NamePointersChunk(std::vector<Chunk *> &v) : chunks(v) {}
  size_t getSize() const override { return chunks.size() * 4; }

  void writeTo(uint8_t *buf) const override {
    for (Chunk *c : chunks) {
      write32le(buf, c->getRVA());
      buf += 4;
    }
  }

private:
  std::vector<Chunk *> chunks;
};

class ExportOrdinalChunk : public NonSectionChunk {
public:
  explicit ExportOrdinalChunk(size_t i) : size(i) {}
  size_t getSize() const override { return size * 2; }

  void writeTo(uint8_t *buf) const override {
    for (Export &e : config->exports) {
      if (e.noname)
        continue;
      write16le(buf, e.ordinal);
      buf += 2;
    }
  }

private:
  size_t size;
};

} // anonymous namespace

void IdataContents::create() {
  std::vector<std::vector<DefinedImportData *>> v = binImports(imports);

  // Create .idata contents for each DLL.
  for (std::vector<DefinedImportData *> &syms : v) {
    // Create lookup and address tables. If they have external names,
    // we need to create HintName chunks to store the names.
    // If they don't (if they are import-by-ordinals), we store only
    // ordinal values to the table.
    size_t base = lookups.size();
    for (DefinedImportData *s : syms) {
      uint16_t ord = s->getOrdinal();
      if (s->getExternalName().empty()) {
        lookups.push_back(make<OrdinalOnlyChunk>(ord));
        addresses.push_back(make<OrdinalOnlyChunk>(ord));
        continue;
      }
      auto *c = make<HintNameChunk>(s->getExternalName(), ord);
      lookups.push_back(make<LookupChunk>(c));
      addresses.push_back(make<LookupChunk>(c));
      hints.push_back(c);
    }
    // Terminate with null values.
    lookups.push_back(make<NullChunk>(config->wordsize));
    addresses.push_back(make<NullChunk>(config->wordsize));

    for (int i = 0, e = syms.size(); i < e; ++i)
      syms[i]->setLocation(addresses[base + i]);

    // Create the import table header.
    dllNames.push_back(make<StringChunk>(syms[0]->getDLLName()));
    auto *dir = make<ImportDirectoryChunk>(dllNames.back());
    dir->lookupTab = lookups[base];
    dir->addressTab = addresses[base];
    dirs.push_back(dir);
  }
  // Add null terminator.
  dirs.push_back(make<NullChunk>(sizeof(ImportDirectoryTableEntry)));
}

std::vector<Chunk *> DelayLoadContents::getChunks() {
  std::vector<Chunk *> v;
  v.insert(v.end(), dirs.begin(), dirs.end());
  v.insert(v.end(), names.begin(), names.end());
  v.insert(v.end(), hintNames.begin(), hintNames.end());
  v.insert(v.end(), dllNames.begin(), dllNames.end());
  return v;
}

std::vector<Chunk *> DelayLoadContents::getDataChunks() {
  std::vector<Chunk *> v;
  v.insert(v.end(), moduleHandles.begin(), moduleHandles.end());
  v.insert(v.end(), addresses.begin(), addresses.end());
  return v;
}

uint64_t DelayLoadContents::getDirSize() {
  return dirs.size() * sizeof(delay_import_directory_table_entry);
}

void DelayLoadContents::create(Defined *h) {
  helper = h;
  std::vector<std::vector<DefinedImportData *>> v = binImports(imports);

  // Create .didat contents for each DLL.
  for (std::vector<DefinedImportData *> &syms : v) {
    // Create the delay import table header.
    dllNames.push_back(make<StringChunk>(syms[0]->getDLLName()));
    auto *dir = make<DelayDirectoryChunk>(dllNames.back());

    size_t base = addresses.size();
    for (DefinedImportData *s : syms) {
      Chunk *t = newThunkChunk(s, dir);
      auto *a = make<DelayAddressChunk>(t);
      addresses.push_back(a);
      thunks.push_back(t);
      StringRef extName = s->getExternalName();
      if (extName.empty()) {
        names.push_back(make<OrdinalOnlyChunk>(s->getOrdinal()));
      } else {
        auto *c = make<HintNameChunk>(extName, 0);
        names.push_back(make<LookupChunk>(c));
        hintNames.push_back(c);
      }
    }
    // Terminate with null values.
    addresses.push_back(make<NullChunk>(8));
    names.push_back(make<NullChunk>(8));

    for (int i = 0, e = syms.size(); i < e; ++i)
      syms[i]->setLocation(addresses[base + i]);
    auto *mh = make<NullChunk>(8);
    mh->setAlignment(8);
    moduleHandles.push_back(mh);

    // Fill the delay import table header fields.
    dir->moduleHandle = mh;
    dir->addressTab = addresses[base];
    dir->nameTab = names[base];
    dirs.push_back(dir);
  }
  // Add null terminator.
  dirs.push_back(make<NullChunk>(sizeof(delay_import_directory_table_entry)));
}

Chunk *DelayLoadContents::newThunkChunk(DefinedImportData *s, Chunk *dir) {
  switch (config->machine) {
  case AMD64:
    return make<ThunkChunkX64>(s, dir, helper);
  case I386:
    return make<ThunkChunkX86>(s, dir, helper);
  case ARMNT:
    return make<ThunkChunkARM>(s, dir, helper);
  case ARM64:
    return make<ThunkChunkARM64>(s, dir, helper);
  default:
    llvm_unreachable("unsupported machine type");
  }
}

EdataContents::EdataContents() {
  uint16_t maxOrdinal = 0;
  for (Export &e : config->exports)
    maxOrdinal = std::max(maxOrdinal, e.ordinal);

  auto *dllName = make<StringChunk>(sys::path::filename(config->outputFile));
  auto *addressTab = make<AddressTableChunk>(maxOrdinal);
  std::vector<Chunk *> names;
  for (Export &e : config->exports)
    if (!e.noname)
      names.push_back(make<StringChunk>(e.exportName));

  std::vector<Chunk *> forwards;
  for (Export &e : config->exports) {
    if (e.forwardTo.empty())
      continue;
    e.forwardChunk = make<StringChunk>(e.forwardTo);
    forwards.push_back(e.forwardChunk);
  }

  auto *nameTab = make<NamePointersChunk>(names);
  auto *ordinalTab = make<ExportOrdinalChunk>(names.size());
  auto *dir = make<ExportDirectoryChunk>(maxOrdinal, names.size(), dllName,
                                         addressTab, nameTab, ordinalTab);
  chunks.push_back(dir);
  chunks.push_back(dllName);
  chunks.push_back(addressTab);
  chunks.push_back(nameTab);
  chunks.push_back(ordinalTab);
  chunks.insert(chunks.end(), names.begin(), names.end());
  chunks.insert(chunks.end(), forwards.begin(), forwards.end());
}

} // namespace coff
} // namespace lld
