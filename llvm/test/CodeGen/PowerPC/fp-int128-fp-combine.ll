; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -O0 -mtriple=powerpc64le-unknown-linux-gnu < %s | FileCheck %s

; xscvdpsxds should NOT be emitted, since it saturates the result down to i64.
; We can't use friz here because it may return -0.0 where the original code doesn't.

define float @f_i128_f(float %v) nounwind {
; CHECK-LABEL: f_i128_f:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    mflr 0
; CHECK-NEXT:    std 0, 16(1)
; CHECK-NEXT:    stdu 1, -32(1)
; CHECK-NEXT:    bl __fixsfti
; CHECK-NEXT:    nop
; CHECK-NEXT:    bl __floattisf
; CHECK-NEXT:    nop
; CHECK-NEXT:    addi 1, 1, 32
; CHECK-NEXT:    ld 0, 16(1)
; CHECK-NEXT:    mtlr 0
; CHECK-NEXT:    blr
entry:
  %a = fptosi float %v to i128
  %b = sitofp i128 %a to float
  ret float %b
}

; NSZ, so it's safe to friz.

define float @f_i128_fi_nsz(float %v) #0 {
; CHECK-LABEL: f_i128_fi_nsz:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    friz 0, 1
; CHECK-NEXT:    fmr 1, 0
; CHECK-NEXT:    blr
entry:
  %a = fptosi float %v to i128
  %b = sitofp i128 %a to float
  ret float %b
}

attributes #0 = { "no-signed-zeros-fp-math"="true" }

