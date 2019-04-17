; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -instcombine -S | FileCheck %s
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare void @llvm.assume(i1) #1

define i32 @test1(i32 %a) #0 {
; CHECK-LABEL: @test1(
; CHECK-NEXT:    [[AND:%.*]] = and i32 [[A:%.*]], 15
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[AND]], 5
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 5
;
  %and = and i32 %a, 15
  %cmp = icmp eq i32 %and, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 7
  ret i32 %and1
}

define i32 @test2(i32 %a) #0 {
; CHECK-LABEL: @test2(
; CHECK-NEXT:    [[AND:%.*]] = and i32 [[A:%.*]], 15
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[AND]], 10
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 2
;
  %and = and i32 %a, 15
  %nand = xor i32 %and, -1
  %cmp = icmp eq i32 %nand, 4294967285
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 7
  ret i32 %and1
}

define i32 @test3(i32 %a) #0 {
; CHECK-LABEL: @test3(
; CHECK-NEXT:    [[V:%.*]] = or i32 [[A:%.*]], -16
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[V]], -11
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 5
;
  %v = or i32 %a, 4294967280
  %cmp = icmp eq i32 %v, 4294967285
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 7
  ret i32 %and1
}

define i32 @test4(i32 %a) #0 {
; CHECK-LABEL: @test4(
; CHECK-NEXT:    [[V:%.*]] = or i32 [[A:%.*]], -16
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[V]], -6
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 2
;
  %v = or i32 %a, 4294967280
  %nv = xor i32 %v, -1
  %cmp = icmp eq i32 %nv, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 7
  ret i32 %and1
}

define i32 @test5(i32 %a) #0 {
; CHECK-LABEL: @test5(
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[A:%.*]], 4
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 4
;
  %v = xor i32 %a, 1
  %cmp = icmp eq i32 %v, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 7
  ret i32 %and1
}

define i32 @test6(i32 %a) #0 {
; CHECK-LABEL: @test6(
; CHECK-NEXT:    [[V_MASK:%.*]] = and i32 [[A:%.*]], 1073741823
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[V_MASK]], 5
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 5
;
  %v = shl i32 %a, 2
  %cmp = icmp eq i32 %v, 20
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 63
  ret i32 %and1
}

define i32 @test7(i32 %a) #0 {
; CHECK-LABEL: @test7(
; CHECK-NEXT:    [[V_MASK:%.*]] = and i32 [[A:%.*]], -4
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[V_MASK]], 20
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 20
;
  %v = lshr i32 %a, 2
  %cmp = icmp eq i32 %v, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 252
  ret i32 %and1
}

define i32 @test8(i32 %a) #0 {
; CHECK-LABEL: @test8(
; CHECK-NEXT:    [[V_MASK:%.*]] = and i32 [[A:%.*]], -4
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i32 [[V_MASK]], 20
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 20
;
  %v = lshr i32 %a, 2
  %cmp = icmp eq i32 %v, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 252
  ret i32 %and1
}

define i32 @test9(i32 %a) #0 {
; CHECK-LABEL: @test9(
; CHECK-NEXT:    [[CMP:%.*]] = icmp sgt i32 [[A:%.*]], 5
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 0
;
  %cmp = icmp sgt i32 %a, 5
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 2147483648
  ret i32 %and1
}

define i32 @test10(i32 %a) #0 {
; CHECK-LABEL: @test10(
; CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[A:%.*]], -1
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 -2147483648
;
  %cmp = icmp sle i32 %a, -2
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 2147483648
  ret i32 %and1
}

define i32 @test11(i32 %a) #0 {
; CHECK-LABEL: @test11(
; CHECK-NEXT:    [[CMP:%.*]] = icmp ult i32 [[A:%.*]], 257
; CHECK-NEXT:    tail call void @llvm.assume(i1 [[CMP]])
; CHECK-NEXT:    ret i32 0
;
  %cmp = icmp ule i32 %a, 256
  tail call void @llvm.assume(i1 %cmp)
  %and1 = and i32 %a, 3072
  ret i32 %and1
}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind }

