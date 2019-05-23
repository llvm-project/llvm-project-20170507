; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=riscv32 -mattr=+m | FileCheck %s --check-prefix=RISCV32

; Test ADDCARRY node expansion on a target that does not currently support ADDCARRY.
; Signed fixed point multiplication eventually expands down to an ADDCARRY.

declare  i64 @llvm.smul.fix.i64  (i64, i64, i32)

define i64 @addcarry(i64 %x, i64 %y) nounwind {
; RISCV32-LABEL: addcarry:
; RISCV32:       # %bb.0:
; RISCV32-NEXT:    mul a4, a0, a3
; RISCV32-NEXT:    mulhu a5, a0, a2
; RISCV32-NEXT:    add a4, a5, a4
; RISCV32-NEXT:    sltu a6, a4, a5
; RISCV32-NEXT:    mulhu a5, a0, a3
; RISCV32-NEXT:    add a6, a5, a6
; RISCV32-NEXT:    mulhu a5, a1, a2
; RISCV32-NEXT:    add a7, a6, a5
; RISCV32-NEXT:    mul a5, a1, a2
; RISCV32-NEXT:    add a6, a4, a5
; RISCV32-NEXT:    sltu a4, a6, a4
; RISCV32-NEXT:    add a4, a7, a4
; RISCV32-NEXT:    mul a5, a1, a3
; RISCV32-NEXT:    add a5, a4, a5
; RISCV32-NEXT:    bgez a1, .LBB0_2
; RISCV32-NEXT:  # %bb.1:
; RISCV32-NEXT:    sub a5, a5, a2
; RISCV32-NEXT:  .LBB0_2:
; RISCV32-NEXT:    bgez a3, .LBB0_4
; RISCV32-NEXT:  # %bb.3:
; RISCV32-NEXT:    sub a5, a5, a0
; RISCV32-NEXT:  .LBB0_4:
; RISCV32-NEXT:    mul a0, a0, a2
; RISCV32-NEXT:    srli a0, a0, 2
; RISCV32-NEXT:    slli a1, a6, 30
; RISCV32-NEXT:    or a0, a0, a1
; RISCV32-NEXT:    srli a1, a6, 2
; RISCV32-NEXT:    slli a2, a5, 30
; RISCV32-NEXT:    or a1, a1, a2
; RISCV32-NEXT:    ret
  %tmp = call i64 @llvm.smul.fix.i64(i64 %x, i64 %y, i32 2);
  ret i64 %tmp;
}
