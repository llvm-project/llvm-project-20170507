; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=riscv32 < %s | FileCheck -check-prefix=RV32 %s
; RUN: llc -mtriple=riscv64 < %s | FileCheck -check-prefix=RV64 %s

define void @foo(i32 signext %size) {
; RV32-LABEL: foo:
; RV32:       # %bb.0: # %entry
; RV32-NEXT:    addi sp, sp, -16
; RV32-NEXT:    .cfi_def_cfa_offset 16
; RV32-NEXT:    sw ra, 12(sp)
; RV32-NEXT:    sw s0, 8(sp)
; RV32-NEXT:    .cfi_offset ra, -4
; RV32-NEXT:    .cfi_offset s0, -8
; RV32-NEXT:    addi s0, sp, 16
; RV32-NEXT:    .cfi_def_cfa s0, 0
; RV32-NEXT:    addi a0, a0, 15
; RV32-NEXT:    andi a0, a0, -16
; RV32-NEXT:    sub a0, sp, a0
; RV32-NEXT:    mv sp, a0
; RV32-NEXT:    call bar
; RV32-NEXT:    addi sp, s0, -16
; RV32-NEXT:    lw s0, 8(sp)
; RV32-NEXT:    .cfi_def_cfa sp, 16
; RV32-NEXT:    lw ra, 12(sp)
; RV32-NEXT:    .cfi_restore ra
; RV32-NEXT:    .cfi_restore s0
; RV32-NEXT:    addi sp, sp, 16
; RV32-NEXT:    .cfi_def_cfa_offset 0
; RV32-NEXT:    ret
;
; RV64-LABEL: foo:
; RV64:       # %bb.0: # %entry
; RV64-NEXT:    addi sp, sp, -16
; RV64-NEXT:    .cfi_def_cfa_offset 16
; RV64-NEXT:    sd ra, 8(sp)
; RV64-NEXT:    sd s0, 0(sp)
; RV64-NEXT:    .cfi_offset ra, -8
; RV64-NEXT:    .cfi_offset s0, -16
; RV64-NEXT:    addi s0, sp, 16
; RV64-NEXT:    .cfi_def_cfa s0, 0
; RV64-NEXT:    slli a0, a0, 32
; RV64-NEXT:    srli a0, a0, 32
; RV64-NEXT:    addi a0, a0, 15
; RV64-NEXT:    addi a1, zero, 1
; RV64-NEXT:    slli a1, a1, 33
; RV64-NEXT:    addi a1, a1, -16
; RV64-NEXT:    and a0, a0, a1
; RV64-NEXT:    sub a0, sp, a0
; RV64-NEXT:    mv sp, a0
; RV64-NEXT:    call bar
; RV64-NEXT:    addi sp, s0, -16
; RV64-NEXT:    ld s0, 0(sp)
; RV64-NEXT:    .cfi_def_cfa sp, 16
; RV64-NEXT:    ld ra, 8(sp)
; RV64-NEXT:    .cfi_restore ra
; RV64-NEXT:    .cfi_restore s0
; RV64-NEXT:    addi sp, sp, 16
; RV64-NEXT:    .cfi_def_cfa_offset 0
; RV64-NEXT:    ret
entry:
  %0 = alloca i8, i32 %size, align 16
  call void @bar(i8* nonnull %0) #2
  ret void
}

declare void @bar(i8*)
