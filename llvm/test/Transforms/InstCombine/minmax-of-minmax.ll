; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -S -instcombine < %s | FileCheck %s

define i32 @smax_of_smax_smin_commute0(i32 %x, i32 %y) {
; CHECK-LABEL: @smax_of_smax_smin_commute0(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp slt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp slt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp sgt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp slt i32 %x, %y
  %min = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp slt i32 %y, %x
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp sgt i32 %max, %min
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define i32 @smax_of_smax_smin_commute1(i32 %x, i32 %y) {
; CHECK-LABEL: @smax_of_smax_smin_commute1(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp sgt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp sgt i32 [[X]], [[Y]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp sgt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp sgt i32 %x, %y
  %min = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp sgt i32 %x, %y
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp sgt i32 %max, %min
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define i32 @smax_of_smax_smin_commute2(i32 %x, i32 %y) {
; CHECK-LABEL: @smax_of_smax_smin_commute2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp slt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp slt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp slt i32 [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp slt i32 %x, %y
  %min = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp slt i32 %y, %x
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp slt i32 %min, %max
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define <2 x i32> @smax_of_smax_smin_commute3(<2 x i32> %x, <2 x i32> %y) {
; CHECK-LABEL: @smax_of_smax_smin_commute3(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp sgt <2 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select <2 x i1> [[CMP1]], <2 x i32> [[Y]], <2 x i32> [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp sgt <2 x i32> [[X]], [[Y]]
; CHECK-NEXT:    [[MAX:%.*]] = select <2 x i1> [[CMP2]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp slt <2 x i32> [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select <2 x i1> [[CMP3]], <2 x i32> [[MAX]], <2 x i32> [[MIN]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %cmp1 = icmp sgt <2 x i32> %x, %y
  %min = select <2 x i1> %cmp1, <2 x i32> %y, <2 x i32> %x
  %cmp2 = icmp sgt <2 x i32> %x, %y
  %max = select <2 x i1> %cmp2, <2 x i32> %x, <2 x i32> %y
  %cmp3 = icmp slt <2 x i32> %min, %max
  %r = select <2 x i1> %cmp3, <2 x i32> %max, <2 x i32> %min
  ret <2 x i32> %r
}

define i32 @smin_of_smin_smax_commute0(i32 %x, i32 %y) {
; CHECK-LABEL: @smin_of_smin_smax_commute0(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp sgt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp sgt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp sgt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp sgt i32 %x, %y
  %max = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp sgt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp sgt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define i32 @smin_of_smin_smax_commute1(i32 %x, i32 %y) {
; CHECK-LABEL: @smin_of_smin_smax_commute1(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp slt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp slt i32 [[X]], [[Y]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp sgt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp slt i32 %x, %y
  %max = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp slt i32 %x, %y
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp sgt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define <2 x i32> @smin_of_smin_smax_commute2(<2 x i32> %x, <2 x i32> %y) {
; CHECK-LABEL: @smin_of_smin_smax_commute2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp sgt <2 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select <2 x i1> [[CMP1]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp slt <2 x i32> [[X]], [[Y]]
; CHECK-NEXT:    [[MIN:%.*]] = select <2 x i1> [[CMP2]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp slt <2 x i32> [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select <2 x i1> [[CMP3]], <2 x i32> [[MIN]], <2 x i32> [[MAX]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %cmp1 = icmp sgt <2 x i32> %x, %y
  %max = select <2 x i1> %cmp1, <2 x i32> %x, <2 x i32> %y
  %cmp2 = icmp slt <2 x i32> %x, %y
  %min = select <2 x i1> %cmp2, <2 x i32> %x, <2 x i32> %y
  %cmp3 = icmp slt <2 x i32> %min, %max
  %r = select <2 x i1> %cmp3, <2 x i32> %min, <2 x i32> %max
  ret <2 x i32> %r
}

define i32 @smin_of_smin_smax_commute3(i32 %x, i32 %y) {
; CHECK-LABEL: @smin_of_smin_smax_commute3(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp slt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp sgt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp slt i32 [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp slt i32 %x, %y
  %max = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp sgt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp slt i32 %min, %max
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define i32 @umax_of_umax_umin_commute0(i32 %x, i32 %y) {
; CHECK-LABEL: @umax_of_umax_umin_commute0(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ugt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %y
  %min = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ult i32 %y, %x
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ugt i32 %max, %min
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define i32 @umax_of_umax_umin_commute1(i32 %x, i32 %y) {
; CHECK-LABEL: @umax_of_umax_umin_commute1(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ugt i32 [[X]], [[Y]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ugt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %min = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ugt i32 %x, %y
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ugt i32 %max, %min
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define i32 @umax_of_umax_umin_commute2(i32 %x, i32 %y) {
; CHECK-LABEL: @umax_of_umax_umin_commute2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult i32 [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MAX]], i32 [[MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %y
  %min = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ult i32 %y, %x
  %max = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ult i32 %min, %max
  %r = select i1 %cmp3, i32 %max, i32 %min
  ret i32 %r
}

define <2 x i32> @umax_of_umax_umin_commute3(<2 x i32> %x, <2 x i32> %y) {
; CHECK-LABEL: @umax_of_umax_umin_commute3(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt <2 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select <2 x i1> [[CMP1]], <2 x i32> [[Y]], <2 x i32> [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ugt <2 x i32> [[X]], [[Y]]
; CHECK-NEXT:    [[MAX:%.*]] = select <2 x i1> [[CMP2]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult <2 x i32> [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select <2 x i1> [[CMP3]], <2 x i32> [[MAX]], <2 x i32> [[MIN]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %cmp1 = icmp ugt <2 x i32> %x, %y
  %min = select <2 x i1> %cmp1, <2 x i32> %y, <2 x i32> %x
  %cmp2 = icmp ugt <2 x i32> %x, %y
  %max = select <2 x i1> %cmp2, <2 x i32> %x, <2 x i32> %y
  %cmp3 = icmp ult <2 x i32> %min, %max
  %r = select <2 x i1> %cmp3, <2 x i32> %max, <2 x i32> %min
  ret <2 x i32> %r
}

define i32 @umin_of_umin_umax_commute0(i32 %x, i32 %y) {
; CHECK-LABEL: @umin_of_umin_umax_commute0(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ugt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ugt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %max = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ugt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ugt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define i32 @umin_of_umin_umax_commute1(i32 %x, i32 %y) {
; CHECK-LABEL: @umin_of_umin_umax_commute1(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[X]], [[Y]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ugt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %y
  %max = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ult i32 %x, %y
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ugt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define <2 x i32> @umin_of_umin_umax_commute2(<2 x i32> %x, <2 x i32> %y) {
; CHECK-LABEL: @umin_of_umin_umax_commute2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt <2 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select <2 x i1> [[CMP1]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult <2 x i32> [[X]], [[Y]]
; CHECK-NEXT:    [[MIN:%.*]] = select <2 x i1> [[CMP2]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult <2 x i32> [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select <2 x i1> [[CMP3]], <2 x i32> [[MIN]], <2 x i32> [[MAX]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %cmp1 = icmp ugt <2 x i32> %x, %y
  %max = select <2 x i1> %cmp1, <2 x i32> %x, <2 x i32> %y
  %cmp2 = icmp ult <2 x i32> %x, %y
  %min = select <2 x i1> %cmp2, <2 x i32> %x, <2 x i32> %y
  %cmp3 = icmp ult <2 x i32> %min, %max
  %r = select <2 x i1> %cmp3, <2 x i32> %min, <2 x i32> %max
  ret <2 x i32> %r
}

define i32 @umin_of_umin_umax_commute3(i32 %x, i32 %y) {
; CHECK-LABEL: @umin_of_umin_umax_commute3(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ugt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult i32 [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %y
  %max = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ugt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ult i32 %min, %max
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define i32 @umin_of_smin_umax_wrong_pattern(i32 %x, i32 %y) {
; CHECK-LABEL: @umin_of_smin_umax_wrong_pattern(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp sgt i32 [[Y]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ugt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %max = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp sgt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ugt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define i32 @smin_of_umin_umax_wrong_pattern2(i32 %x, i32 %y) {
; CHECK-LABEL: @smin_of_umin_umax_wrong_pattern2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Y]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[X]], [[Y]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp sgt i32 [[MAX]], [[MIN]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %y
  %max = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ult i32 %x, %y
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp sgt i32 %max, %min
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}

define <2 x i32> @umin_of_umin_umax_wrong_operand(<2 x i32> %x, <2 x i32> %y, <2 x i32> %z) {
; CHECK-LABEL: @umin_of_umin_umax_wrong_operand(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt <2 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select <2 x i1> [[CMP1]], <2 x i32> [[X]], <2 x i32> [[Y]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult <2 x i32> [[X]], [[Z:%.*]]
; CHECK-NEXT:    [[MIN:%.*]] = select <2 x i1> [[CMP2]], <2 x i32> [[X]], <2 x i32> [[Z]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult <2 x i32> [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select <2 x i1> [[CMP3]], <2 x i32> [[MIN]], <2 x i32> [[MAX]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %cmp1 = icmp ugt <2 x i32> %x, %y
  %max = select <2 x i1> %cmp1, <2 x i32> %x, <2 x i32> %y
  %cmp2 = icmp ult <2 x i32> %x, %z
  %min = select <2 x i1> %cmp2, <2 x i32> %x, <2 x i32> %z
  %cmp3 = icmp ult <2 x i32> %min, %max
  %r = select <2 x i1> %cmp3, <2 x i32> %min, <2 x i32> %max
  ret <2 x i32> %r
}

define i32 @umin_of_umin_umax_wrong_operand2(i32 %x, i32 %y, i32 %z) {
; CHECK-LABEL: @umin_of_umin_umax_wrong_operand2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X:%.*]], [[Z:%.*]]
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP1]], i32 [[Z]], i32 [[X]]
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ugt i32 [[Y:%.*]], [[X]]
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], i32 [[X]], i32 [[Y]]
; CHECK-NEXT:    [[CMP3:%.*]] = icmp ult i32 [[MIN]], [[MAX]]
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP3]], i32 [[MIN]], i32 [[MAX]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %cmp1 = icmp ult i32 %x, %z
  %max = select i1 %cmp1, i32 %z, i32 %x
  %cmp2 = icmp ugt i32 %y, %x
  %min = select i1 %cmp2, i32 %x, i32 %y
  %cmp3 = icmp ult i32 %min, %max
  %r = select i1 %cmp3, i32 %min, i32 %max
  ret i32 %r
}
