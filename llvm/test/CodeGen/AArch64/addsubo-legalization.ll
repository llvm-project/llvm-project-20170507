; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=thumbv7k-linux-gnu | FileCheck %s

declare {<2 x i64>, <2 x i1>} @llvm.uadd.with.overflow.v2i64(<2 x i64>, <2 x i64>)
declare {<2 x i64>, <2 x i1>} @llvm.usub.with.overflow.v2i64(<2 x i64>, <2 x i64>)
declare {<2 x i64>, <2 x i1>} @llvm.sadd.with.overflow.v2i64(<2 x i64>, <2 x i64>)
declare {<2 x i64>, <2 x i1>} @llvm.ssub.with.overflow.v2i64(<2 x i64>, <2 x i64>)

define <2 x i1> @uaddo(<2 x i64> *%ptr, <2 x i64> *%ptr2) {
; CHECK-LABEL: uaddo:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    push {r4, r5, r6, r7, lr}
; CHECK-NEXT:    vld1.64 {d18, d19}, [r0]
; CHECK-NEXT:    vld1.64 {d16, d17}, [r1]
; CHECK-NEXT:    movs r1, #0
; CHECK-NEXT:    vadd.i64 q8, q9, q8
; CHECK-NEXT:    vmov.32 r3, d18[0]
; CHECK-NEXT:    vmov.32 r2, d18[1]
; CHECK-NEXT:    vmov.32 r12, d16[0]
; CHECK-NEXT:    vmov.32 lr, d16[1]
; CHECK-NEXT:    vmov.32 r4, d17[0]
; CHECK-NEXT:    vmov.32 r5, d19[0]
; CHECK-NEXT:    vmov.32 r6, d17[1]
; CHECK-NEXT:    vmov.32 r7, d19[1]
; CHECK-NEXT:    subs.w r3, r12, r3
; CHECK-NEXT:    sbcs.w r2, lr, r2
; CHECK-NEXT:    mov.w r2, #0
; CHECK-NEXT:    it lo
; CHECK-NEXT:    movlo r2, #1
; CHECK-NEXT:    cmp r2, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r2, #-1
; CHECK-NEXT:    subs r3, r4, r5
; CHECK-NEXT:    sbcs.w r3, r6, r7
; CHECK-NEXT:    it lo
; CHECK-NEXT:    movlo r1, #1
; CHECK-NEXT:    cmp r1, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r1, #-1
; CHECK-NEXT:    vst1.64 {d16, d17}, [r0]
; CHECK-NEXT:    mov r0, r2
; CHECK-NEXT:    pop {r4, r5, r6, r7, pc}
  %x = load <2 x i64>, <2 x i64>* %ptr, align 8
  %y = load <2 x i64>, <2 x i64>* %ptr2, align 8
  %s = call {<2 x i64>, <2 x i1>} @llvm.uadd.with.overflow.v2i64(<2 x i64> %x, <2 x i64> %y)
  %m = extractvalue {<2 x i64>, <2 x i1>} %s, 0
  %o = extractvalue {<2 x i64>, <2 x i1>} %s, 1
  store <2 x i64> %m, <2 x i64>* %ptr
  ret <2 x i1> %o
}

define <2 x i1> @usubo(<2 x i64> *%ptr, <2 x i64> *%ptr2) {
; CHECK-LABEL: usubo:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    push {r4, r5, r6, r7, lr}
; CHECK-NEXT:    vld1.64 {d16, d17}, [r1]
; CHECK-NEXT:    movs r1, #0
; CHECK-NEXT:    vld1.64 {d18, d19}, [r0]
; CHECK-NEXT:    vsub.i64 q8, q9, q8
; CHECK-NEXT:    vmov.32 r12, d18[0]
; CHECK-NEXT:    vmov.32 lr, d18[1]
; CHECK-NEXT:    vmov.32 r3, d16[0]
; CHECK-NEXT:    vmov.32 r2, d16[1]
; CHECK-NEXT:    vmov.32 r4, d19[0]
; CHECK-NEXT:    vmov.32 r5, d17[0]
; CHECK-NEXT:    vmov.32 r6, d19[1]
; CHECK-NEXT:    vmov.32 r7, d17[1]
; CHECK-NEXT:    subs.w r3, r12, r3
; CHECK-NEXT:    sbcs.w r2, lr, r2
; CHECK-NEXT:    mov.w r2, #0
; CHECK-NEXT:    it lo
; CHECK-NEXT:    movlo r2, #1
; CHECK-NEXT:    cmp r2, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r2, #-1
; CHECK-NEXT:    subs r3, r4, r5
; CHECK-NEXT:    sbcs.w r3, r6, r7
; CHECK-NEXT:    it lo
; CHECK-NEXT:    movlo r1, #1
; CHECK-NEXT:    cmp r1, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r1, #-1
; CHECK-NEXT:    vst1.64 {d16, d17}, [r0]
; CHECK-NEXT:    mov r0, r2
; CHECK-NEXT:    pop {r4, r5, r6, r7, pc}
  %x = load <2 x i64>, <2 x i64>* %ptr, align 8
  %y = load <2 x i64>, <2 x i64>* %ptr2, align 8
  %s = call {<2 x i64>, <2 x i1>} @llvm.usub.with.overflow.v2i64(<2 x i64> %x, <2 x i64> %y)
  %m = extractvalue {<2 x i64>, <2 x i1>} %s, 0
  %o = extractvalue {<2 x i64>, <2 x i1>} %s, 1
  store <2 x i64> %m, <2 x i64>* %ptr
  ret <2 x i1> %o
}

define <2 x i1> @saddo(<2 x i64> *%ptr, <2 x i64> *%ptr2) {
; CHECK-LABEL: saddo:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    push {r4, r5, r6, r7, lr}
; CHECK-NEXT:    vld1.64 {d16, d17}, [r1]
; CHECK-NEXT:    movs r5, #0
; CHECK-NEXT:    movs r6, #0
; CHECK-NEXT:    movs r3, #0
; CHECK-NEXT:    vmov.32 r1, d16[1]
; CHECK-NEXT:    vld1.64 {d18, d19}, [r0]
; CHECK-NEXT:    vmov.32 r2, d17[1]
; CHECK-NEXT:    vadd.i64 q8, q9, q8
; CHECK-NEXT:    vmov.32 r12, d18[1]
; CHECK-NEXT:    vmov.32 r4, d19[1]
; CHECK-NEXT:    vmov.32 lr, d16[1]
; CHECK-NEXT:    vmov.32 r7, d17[1]
; CHECK-NEXT:    cmp.w r1, #-1
; CHECK-NEXT:    mov.w r1, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r1, #1
; CHECK-NEXT:    cmp r1, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r1, #-1
; CHECK-NEXT:    cmp.w r2, #-1
; CHECK-NEXT:    mov.w r2, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r2, #1
; CHECK-NEXT:    cmp.w r12, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r5, #1
; CHECK-NEXT:    cmp r5, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r5, #-1
; CHECK-NEXT:    cmp.w r4, #-1
; CHECK-NEXT:    mov.w r4, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r4, #1
; CHECK-NEXT:    cmp.w lr, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r6, #1
; CHECK-NEXT:    cmp r6, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r6, #-1
; CHECK-NEXT:    cmp.w r7, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r3, #1
; CHECK-NEXT:    cmp r3, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r3, #-1
; CHECK-NEXT:    cmp r4, #0
; CHECK-NEXT:    vdup.32 d19, r3
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r4, #-1
; CHECK-NEXT:    cmp r2, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r2, #-1
; CHECK-NEXT:    vdup.32 d23, r2
; CHECK-NEXT:    vdup.32 d21, r4
; CHECK-NEXT:    vdup.32 d18, r6
; CHECK-NEXT:    vdup.32 d22, r1
; CHECK-NEXT:    vdup.32 d20, r5
; CHECK-NEXT:    vceq.i32 q9, q10, q9
; CHECK-NEXT:    vst1.64 {d16, d17}, [r0]
; CHECK-NEXT:    vceq.i32 q10, q10, q11
; CHECK-NEXT:    vrev64.32 q11, q9
; CHECK-NEXT:    vrev64.32 q12, q10
; CHECK-NEXT:    vand q9, q9, q11
; CHECK-NEXT:    vand q10, q10, q12
; CHECK-NEXT:    vbic q9, q10, q9
; CHECK-NEXT:    vmovn.i64 d18, q9
; CHECK-NEXT:    vmov r2, r1, d18
; CHECK-NEXT:    mov r0, r2
; CHECK-NEXT:    pop {r4, r5, r6, r7, pc}
  %x = load <2 x i64>, <2 x i64>* %ptr, align 8
  %y = load <2 x i64>, <2 x i64>* %ptr2, align 8
  %s = call {<2 x i64>, <2 x i1>} @llvm.sadd.with.overflow.v2i64(<2 x i64> %x, <2 x i64> %y)
  %m = extractvalue {<2 x i64>, <2 x i1>} %s, 0
  %o = extractvalue {<2 x i64>, <2 x i1>} %s, 1
  store <2 x i64> %m, <2 x i64>* %ptr
  ret <2 x i1> %o
}

define <2 x i1> @ssubo(<2 x i64> *%ptr, <2 x i64> *%ptr2) {
; CHECK-LABEL: ssubo:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    push {r4, r5, r6, r7, lr}
; CHECK-NEXT:    vld1.64 {d18, d19}, [r1]
; CHECK-NEXT:    movs r5, #0
; CHECK-NEXT:    movs r6, #0
; CHECK-NEXT:    movs r3, #0
; CHECK-NEXT:    vld1.64 {d20, d21}, [r0]
; CHECK-NEXT:    vsub.i64 q8, q10, q9
; CHECK-NEXT:    vmov.32 r12, d20[1]
; CHECK-NEXT:    vmov.32 lr, d21[1]
; CHECK-NEXT:    vmov.32 r1, d16[1]
; CHECK-NEXT:    vmov.32 r2, d17[1]
; CHECK-NEXT:    vmov.32 r4, d18[1]
; CHECK-NEXT:    vmov.32 r7, d19[1]
; CHECK-NEXT:    cmp.w r1, #-1
; CHECK-NEXT:    mov.w r1, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r1, #1
; CHECK-NEXT:    cmp r1, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r1, #-1
; CHECK-NEXT:    cmp.w r2, #-1
; CHECK-NEXT:    mov.w r2, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r2, #1
; CHECK-NEXT:    cmp.w r12, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r5, #1
; CHECK-NEXT:    cmp r5, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r5, #-1
; CHECK-NEXT:    cmp.w lr, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r6, #1
; CHECK-NEXT:    cmp.w r4, #-1
; CHECK-NEXT:    mov.w r4, #0
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r4, #1
; CHECK-NEXT:    cmp r4, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r4, #-1
; CHECK-NEXT:    cmp.w r7, #-1
; CHECK-NEXT:    it gt
; CHECK-NEXT:    movgt r3, #1
; CHECK-NEXT:    cmp r3, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r3, #-1
; CHECK-NEXT:    vdup.32 d19, r3
; CHECK-NEXT:    cmp r6, #0
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r6, #-1
; CHECK-NEXT:    vdup.32 d21, r6
; CHECK-NEXT:    cmp r2, #0
; CHECK-NEXT:    vdup.32 d18, r4
; CHECK-NEXT:    it ne
; CHECK-NEXT:    movne.w r2, #-1
; CHECK-NEXT:    vdup.32 d23, r2
; CHECK-NEXT:    vdup.32 d20, r5
; CHECK-NEXT:    vdup.32 d22, r1
; CHECK-NEXT:    vceq.i32 q9, q10, q9
; CHECK-NEXT:    vst1.64 {d16, d17}, [r0]
; CHECK-NEXT:    vceq.i32 q10, q10, q11
; CHECK-NEXT:    vrev64.32 q11, q9
; CHECK-NEXT:    vrev64.32 q12, q10
; CHECK-NEXT:    vand q9, q9, q11
; CHECK-NEXT:    vand q10, q10, q12
; CHECK-NEXT:    vmvn q9, q9
; CHECK-NEXT:    vbic q9, q9, q10
; CHECK-NEXT:    vmovn.i64 d18, q9
; CHECK-NEXT:    vmov r2, r1, d18
; CHECK-NEXT:    mov r0, r2
; CHECK-NEXT:    pop {r4, r5, r6, r7, pc}
  %x = load <2 x i64>, <2 x i64>* %ptr, align 8
  %y = load <2 x i64>, <2 x i64>* %ptr2, align 8
  %s = call {<2 x i64>, <2 x i1>} @llvm.ssub.with.overflow.v2i64(<2 x i64> %x, <2 x i64> %y)
  %m = extractvalue {<2 x i64>, <2 x i1>} %s, 0
  %o = extractvalue {<2 x i64>, <2 x i1>} %s, 1
  store <2 x i64> %m, <2 x i64>* %ptr
  ret <2 x i1> %o
}
