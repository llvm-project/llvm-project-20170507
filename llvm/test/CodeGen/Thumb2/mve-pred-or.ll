; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-arm-none-eabi -mattr=+mve -verify-machineinstrs %s -o - | FileCheck %s

define arm_aapcs_vfpcc <4 x i32> @cmpeqz_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpeqz_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.i32 ne, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp eq <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpnez_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpnez_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.i32 eq, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ne <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsltz_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpsltz_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 ge, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp slt <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsgtz_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpsgtz_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 le, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sgt <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpslez_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpslez_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 gt, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sle <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsgez_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpsgez_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 lt, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sge <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpultz_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpultz_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ult <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpugtz_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpugtz_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.i32 eq, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ugt <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpulez_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpulez_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.u32 cs, q1, zr
; CHECK-NEXT:    vmrs r0, p0
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vmrs r1, p0
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmsr p0, r0
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ule <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpugez_v4i1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: cmpugez_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp uge <4 x i32> %b, zeroinitializer
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}



define arm_aapcs_vfpcc <4 x i32> @cmpeq_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpeq_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.i32 ne, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp eq <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpne_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpne_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.i32 eq, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ne <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpslt_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpslt_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 le, q2, q1
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp slt <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsgt_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpsgt_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 le, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sgt <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsle_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpsle_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 lt, q2, q1
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sle <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpsge_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpsge_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i32 ne, q0, zr
; CHECK-NEXT:    vcmpt.s32 lt, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp sge <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpult_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpult_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.u32 hi, q2, q1
; CHECK-NEXT:    vmrs r0, p0
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vmrs r1, p0
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmsr p0, r0
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ult <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpugt_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpugt_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.u32 hi, q1, q2
; CHECK-NEXT:    vmrs r0, p0
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vmrs r1, p0
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmsr p0, r0
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ugt <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpule_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpule_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.u32 cs, q2, q1
; CHECK-NEXT:    vmrs r0, p0
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vmrs r1, p0
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmsr p0, r0
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp ule <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}

define arm_aapcs_vfpcc <4 x i32> @cmpuge_v4i1(<4 x i32> %a, <4 x i32> %b, <4 x i32> %c) {
; CHECK-LABEL: cmpuge_v4i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vcmp.u32 cs, q1, q2
; CHECK-NEXT:    vmrs r0, p0
; CHECK-NEXT:    vcmp.i32 eq, q0, zr
; CHECK-NEXT:    vmrs r1, p0
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmsr p0, r0
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <4 x i32> %a, zeroinitializer
  %c2 = icmp uge <4 x i32> %b, %c
  %o = or <4 x i1> %c1, %c2
  %s = select <4 x i1> %o, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %s
}




define arm_aapcs_vfpcc <8 x i16> @cmpeqz_v8i1(<8 x i16> %a, <8 x i16> %b) {
; CHECK-LABEL: cmpeqz_v8i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i16 ne, q0, zr
; CHECK-NEXT:    vcmpt.i16 ne, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <8 x i16> %a, zeroinitializer
  %c2 = icmp eq <8 x i16> %b, zeroinitializer
  %o = or <8 x i1> %c1, %c2
  %s = select <8 x i1> %o, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %s
}

define arm_aapcs_vfpcc <8 x i16> @cmpeq_v8i1(<8 x i16> %a, <8 x i16> %b, <8 x i16> %c) {
; CHECK-LABEL: cmpeq_v8i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i16 ne, q0, zr
; CHECK-NEXT:    vcmpt.i16 ne, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <8 x i16> %a, zeroinitializer
  %c2 = icmp eq <8 x i16> %b, %c
  %o = or <8 x i1> %c1, %c2
  %s = select <8 x i1> %o, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %s
}


define arm_aapcs_vfpcc <16 x i8> @cmpeqz_v16i1(<16 x i8> %a, <16 x i8> %b) {
; CHECK-LABEL: cmpeqz_v16i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i8 ne, q0, zr
; CHECK-NEXT:    vcmpt.i8 ne, q1, zr
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <16 x i8> %a, zeroinitializer
  %c2 = icmp eq <16 x i8> %b, zeroinitializer
  %o = or <16 x i1> %c1, %c2
  %s = select <16 x i1> %o, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %s
}

define arm_aapcs_vfpcc <16 x i8> @cmpeq_v16i1(<16 x i8> %a, <16 x i8> %b, <16 x i8> %c) {
; CHECK-LABEL: cmpeq_v16i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vpt.i8 ne, q0, zr
; CHECK-NEXT:    vcmpt.i8 ne, q1, q2
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpsel q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <16 x i8> %a, zeroinitializer
  %c2 = icmp eq <16 x i8> %b, %c
  %o = or <16 x i1> %c1, %c2
  %s = select <16 x i1> %o, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %s
}


define arm_aapcs_vfpcc <2 x i64> @cmpeqz_v2i1(<2 x i64> %a, <2 x i64> %b) {
; CHECK-LABEL: cmpeqz_v2i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov r0, s5
; CHECK-NEXT:    vmov r1, s4
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s6
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q2[0], r0
; CHECK-NEXT:    vmov.32 q2[1], r0
; CHECK-NEXT:    vmov r0, s7
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s0
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q2[2], r0
; CHECK-NEXT:    vmov.32 q2[3], r0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s2
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q3[0], r0
; CHECK-NEXT:    vmov.32 q3[1], r0
; CHECK-NEXT:    vmov r0, s3
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q3[2], r0
; CHECK-NEXT:    vmov.32 q3[3], r0
; CHECK-NEXT:    vorr q2, q3, q2
; CHECK-NEXT:    vbic q1, q1, q2
; CHECK-NEXT:    vand q0, q0, q2
; CHECK-NEXT:    vorr q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <2 x i64> %a, zeroinitializer
  %c2 = icmp eq <2 x i64> %b, zeroinitializer
  %o = or <2 x i1> %c1, %c2
  %s = select <2 x i1> %o, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %s
}

define arm_aapcs_vfpcc <2 x i64> @cmpeq_v2i1(<2 x i64> %a, <2 x i64> %b, <2 x i64> %c) {
; CHECK-LABEL: cmpeq_v2i1:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov r0, s9
; CHECK-NEXT:    vmov r1, s5
; CHECK-NEXT:    vmov r2, s4
; CHECK-NEXT:    eors r0, r1
; CHECK-NEXT:    vmov r1, s8
; CHECK-NEXT:    eors r1, r2
; CHECK-NEXT:    vmov r2, s6
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s7
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q3[0], r0
; CHECK-NEXT:    vmov.32 q3[1], r0
; CHECK-NEXT:    vmov r0, s11
; CHECK-NEXT:    eors r0, r1
; CHECK-NEXT:    vmov r1, s10
; CHECK-NEXT:    eors r1, r2
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s0
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q3[2], r0
; CHECK-NEXT:    vmov.32 q3[3], r0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    vmov r1, s2
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q2[0], r0
; CHECK-NEXT:    vmov.32 q2[1], r0
; CHECK-NEXT:    vmov r0, s3
; CHECK-NEXT:    orrs r0, r1
; CHECK-NEXT:    cset r0, eq
; CHECK-NEXT:    tst.w r0, #1
; CHECK-NEXT:    csetm r0, ne
; CHECK-NEXT:    vmov.32 q2[2], r0
; CHECK-NEXT:    vmov.32 q2[3], r0
; CHECK-NEXT:    vorr q2, q2, q3
; CHECK-NEXT:    vbic q1, q1, q2
; CHECK-NEXT:    vand q0, q0, q2
; CHECK-NEXT:    vorr q0, q0, q1
; CHECK-NEXT:    bx lr
entry:
  %c1 = icmp eq <2 x i64> %a, zeroinitializer
  %c2 = icmp eq <2 x i64> %b, %c
  %o = or <2 x i1> %c1, %c2
  %s = select <2 x i1> %o, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %s
}


