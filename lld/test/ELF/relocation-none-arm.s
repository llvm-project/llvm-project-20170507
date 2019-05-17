# RUN: llvm-mc -filetype=obj -triple=armv7-linux-musl %s -o %t.o
# RUN: ld.lld --gc-sections %t.o -o %t
# RUN: llvm-readelf -S -r %t | FileCheck %s

# Test that we discard R_ARM_NONE, but respect the references it creates among
# sections.

# CHECK: .data
# CHECK: There are no relocations in this file.

.globl _start
_start:
  nop
  .reloc 0, R_ARM_NONE, .data

.data
  .long 0
