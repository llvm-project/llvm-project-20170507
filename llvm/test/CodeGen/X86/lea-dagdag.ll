; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-- -mattr=+slow-3ops-lea | FileCheck %s -check-prefixes=CHECK,SLOW
; RUN: llc < %s -mtriple=x86_64-- -mattr=-slow-3ops-lea | FileCheck %s -check-prefixes=CHECK,FAST

define i16 @and_i8_zext_shl_add_i16(i16 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_zext_shl_add_i16:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    # kill: def $edi killed $edi def $rdi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leal (%rdi,%rsi,4), %eax
; CHECK-NEXT:    # kill: def $ax killed $ax killed $eax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %t5 = zext i8 %t4 to i16
  %sh = shl i16 %t5, 2
  %t6 = add i16 %sh, %t0
  ret i16 %t6
}

define i16 @and_i8_shl_zext_add_i16(i16 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_shl_zext_add_i16:
; CHECK:       # %bb.0:
; CHECK-NEXT:    andb $8, %sil
; CHECK-NEXT:    shlb $2, %sil
; CHECK-NEXT:    movzbl %sil, %eax
; CHECK-NEXT:    addl %edi, %eax
; CHECK-NEXT:    # kill: def $ax killed $ax killed $eax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %sh = shl i8 %t4, 2
  %t5 = zext i8 %sh to i16
  %t6 = add i16 %t5, %t0
  ret i16 %t6
}

define i32 @and_i8_zext_shl_add_i32(i32 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_zext_shl_add_i32:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    # kill: def $edi killed $edi def $rdi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leal (%rdi,%rsi,8), %eax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %t5 = zext i8 %t4 to i32
  %sh = shl i32 %t5, 3
  %t6 = add i32 %sh, %t0
  ret i32 %t6
}

define i32 @and_i8_shl_zext_add_i32(i32 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_shl_zext_add_i32:
; CHECK:       # %bb.0:
; CHECK-NEXT:    andb $8, %sil
; CHECK-NEXT:    shlb $3, %sil
; CHECK-NEXT:    movzbl %sil, %eax
; CHECK-NEXT:    addl %edi, %eax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %sh = shl i8 %t4, 3
  %t5 = zext i8 %sh to i32
  %t6 = add i32 %t5, %t0
  ret i32 %t6
}

define i32 @and_i16_zext_shl_add_i32(i32 %t0, i16 %t1) {
; CHECK-LABEL: and_i16_zext_shl_add_i32:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    # kill: def $edi killed $edi def $rdi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leal (%rdi,%rsi,4), %eax
; CHECK-NEXT:    retq
  %t4 = and i16 %t1, 8
  %t5 = zext i16 %t4 to i32
  %sh = shl i32 %t5, 2
  %t6 = add i32 %sh, %t0
  ret i32 %t6
}

define i32 @and_i16_shl_zext_add_i32(i32 %t0, i16 %t1) {
; CHECK-LABEL: and_i16_shl_zext_add_i32:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    # kill: def $edi killed $edi def $rdi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leal (%rdi,%rsi,4), %eax
; CHECK-NEXT:    retq
  %t4 = and i16 %t1, 8
  %sh = shl i16 %t4, 2
  %t5 = zext i16 %sh to i32
  %t6 = add i32 %t5, %t0
  ret i32 %t6
}

define i64 @and_i8_zext_shl_add_i64(i64 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_zext_shl_add_i64:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leaq (%rdi,%rsi,2), %rax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %t5 = zext i8 %t4 to i64
  %sh = shl i64 %t5, 1
  %t6 = add i64 %sh, %t0
  ret i64 %t6
}

define i64 @and_i8_shl_zext_add_i64(i64 %t0, i8 %t1) {
; CHECK-LABEL: and_i8_shl_zext_add_i64:
; CHECK:       # %bb.0:
; CHECK-NEXT:    andb $8, %sil
; CHECK-NEXT:    addb %sil, %sil
; CHECK-NEXT:    movzbl %sil, %eax
; CHECK-NEXT:    addq %rdi, %rax
; CHECK-NEXT:    retq
  %t4 = and i8 %t1, 8
  %sh = shl i8 %t4, 1
  %t5 = zext i8 %sh to i64
  %t6 = add i64 %t5, %t0
  ret i64 %t6
}

define i64 @and_i32_zext_shl_add_i64(i64 %t0, i32 %t1) {
; CHECK-LABEL: and_i32_zext_shl_add_i64:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leaq (%rdi,%rsi,8), %rax
; CHECK-NEXT:    retq
  %t4 = and i32 %t1, 8
  %t5 = zext i32 %t4 to i64
  %sh = shl i64 %t5, 3
  %t6 = add i64 %sh, %t0
  ret i64 %t6
}

define i64 @and_i32_shl_zext_add_i64(i64 %t0, i32 %t1) {
; CHECK-LABEL: and_i32_shl_zext_add_i64:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    leal (,%rsi,8), %eax
; CHECK-NEXT:    addq %rdi, %rax
; CHECK-NEXT:    retq
  %t4 = and i32 %t1, 8
  %sh = shl i32 %t4, 3
  %t5 = zext i32 %sh to i64
  %t6 = add i64 %t5, %t0
  ret i64 %t6
}

define i64 @and_i32_zext_shl_add_i64_overshift(i64 %t0, i32 %t1) {
; CHECK-LABEL: and_i32_zext_shl_add_i64_overshift:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    shlq $4, %rsi
; CHECK-NEXT:    leaq (%rsi,%rdi), %rax
; CHECK-NEXT:    retq
  %t4 = and i32 %t1, 8
  %t5 = zext i32 %t4 to i64
  %sh = shl i64 %t5, 4
  %t6 = add i64 %sh, %t0
  ret i64 %t6
}

define i64 @and_i32_shl_zext_add_i64_overshift(i64 %t0, i32 %t1) {
; CHECK-LABEL: and_i32_shl_zext_add_i64_overshift:
; CHECK:       # %bb.0:
; CHECK-NEXT:    # kill: def $esi killed $esi def $rsi
; CHECK-NEXT:    andl $8, %esi
; CHECK-NEXT:    shll $4, %esi
; CHECK-NEXT:    leaq (%rsi,%rdi), %rax
; CHECK-NEXT:    retq
  %t4 = and i32 %t1, 8
  %sh = shl i32 %t4, 4
  %t5 = zext i32 %sh to i64
  %t6 = add i64 %t5, %t0
  ret i64 %t6
}
