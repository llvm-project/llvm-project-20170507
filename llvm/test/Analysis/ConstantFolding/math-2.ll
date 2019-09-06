; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -early-cse -S -o - %s | FileCheck %s

declare double @atan2(double, double)
define double @f_atan2() {
; CHECK-LABEL: @f_atan2(
; CHECK-NEXT:    [[RES:%.*]] = tail call fast double @atan2(double 1.000000e+00, double 2.000000e+00)
; CHECK-NEXT:    ret double 0x3FDDAC6{{.+}}
;
  %res = tail call fast double @atan2(double 1.0, double 2.0)
  ret double %res
}

declare float @fmodf(float, float)
define float @f_fmodf() {
; CHECK-LABEL: @f_fmodf(
; CHECK-NEXT:    ret float 1.000000e+00
;
  %res = tail call fast float @fmodf(float 1.0, float 2.0)
  ret float %res
}

declare double @pow(double, double)
define double @f_pow() {
; CHECK-LABEL: @f_pow(
; CHECK-NEXT:    ret double 1.000000e+00
;
  %res = tail call fast double @pow(double 1.0, double 2.0)
  ret double %res
}

declare float @llvm.pow.f32(float, float)
define float @i_powf() {
; CHECK-LABEL: @i_powf(
; CHECK-NEXT:    ret float 1.000000e+00
;
  %res = tail call fast float @llvm.pow.f32(float 1.0, float 2.0)
  ret float %res
}

declare double @llvm.powi.f64(double, i32)
define double @i_powi() {
; CHECK-LABEL: @i_powi(
; CHECK-NEXT:    ret double 1.000000e+00
;
  %res = tail call fast double @llvm.powi.f64(double 1.0, i32 2)
  ret double %res
}
