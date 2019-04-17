; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; Test that the pow() won't get simplified to when it's disabled.
;
; RUN: opt < %s -disable-simplify-libcalls -instcombine -S | FileCheck %s

declare double @llvm.pow.f64(double, double)
declare double @pow(double, double)

define double @test_simplify_unavailable1(double %x) {
; CHECK-LABEL: @test_simplify_unavailable1(
; CHECK-NEXT:    [[RETVAL:%.*]] = call double @llvm.pow.f64(double [[X:%.*]], double 5.000000e-01)
; CHECK-NEXT:    ret double [[RETVAL]]
;
  %retval = call double @llvm.pow.f64(double %x, double 0.5)
  ret double %retval
}

; Shrinking is disabled too.

define float @test_simplify_unavailable2(float %f, float %g) {
; CHECK-LABEL: @test_simplify_unavailable2(
; CHECK-NEXT:    [[DF:%.*]] = fpext float [[F:%.*]] to double
; CHECK-NEXT:    [[DG:%.*]] = fpext float [[G:%.*]] to double
; CHECK-NEXT:    [[CALL:%.*]] = call fast double @pow(double [[DF]], double [[DG]])
; CHECK-NEXT:    [[FR:%.*]] = fptrunc double [[CALL]] to float
; CHECK-NEXT:    ret float [[FR]]
;
  %df = fpext float %f to double
  %dg = fpext float %g to double
  %call = call fast double @pow(double %df, double %dg)
  %fr = fptrunc double %call to float
  ret float %fr
}

; Shrinking is disabled for the intrinsic too.

define float @test_simplify_unavailable3(float %f, float %g) {
; CHECK-LABEL: @test_simplify_unavailable3(
; CHECK-NEXT:    [[DF:%.*]] = fpext float [[F:%.*]] to double
; CHECK-NEXT:    [[DG:%.*]] = fpext float [[G:%.*]] to double
; CHECK-NEXT:    [[CALL:%.*]] = call fast double @llvm.pow.f64(double [[DF]], double [[DG]])
; CHECK-NEXT:    [[FR:%.*]] = fptrunc double [[CALL]] to float
; CHECK-NEXT:    ret float [[FR]]
;
  %df = fpext float %f to double
  %dg = fpext float %g to double
  %call = call fast double @llvm.pow.f64(double %df, double %dg)
  %fr = fptrunc double %call to float
  ret float %fr
}
