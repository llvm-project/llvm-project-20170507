; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=i686-unknown < %s | FileCheck %s --check-prefix=X32
; RUN: llc -mtriple=x86_64-unknown < %s | FileCheck %s --check-prefix=X64

@array = weak global [4 x i32] zeroinitializer

define i32 @test_lshr_and(i32 %x) {
; X32-LABEL: test_lshr_and:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    andl $12, %eax
; X32-NEXT:    movl array(%eax), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_lshr_and:
; X64:       # %bb.0:
; X64-NEXT:    # kill: def $edi killed $edi def $rdi
; X64-NEXT:    shrl $2, %edi
; X64-NEXT:    andl $3, %edi
; X64-NEXT:    movl array(,%rdi,4), %eax
; X64-NEXT:    retq
  %tmp2 = lshr i32 %x, 2
  %tmp3 = and i32 %tmp2, 3
  %tmp4 = getelementptr [4 x i32], [4 x i32]* @array, i32 0, i32 %tmp3
  %tmp5 = load i32, i32* %tmp4, align 4
  ret i32 %tmp5
}

define i32* @test_exact1(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact1:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    sarl %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact1:
; X64:       # %bb.0:
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    sarl $3, %esi
; X64-NEXT:    movslq %esi, %rax
; X64-NEXT:    leaq (%rdx,%rax,4), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = ashr exact i32 %sub, 3
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

define i32* @test_exact2(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact2:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    sarl %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact2:
; X64:       # %bb.0:
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    sarl $3, %esi
; X64-NEXT:    movslq %esi, %rax
; X64-NEXT:    leaq (%rdx,%rax,4), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = ashr exact i32 %sub, 3
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

define i32* @test_exact3(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact3:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact3:
; X64:       # %bb.0:
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    sarl $2, %esi
; X64-NEXT:    movslq %esi, %rax
; X64-NEXT:    leaq (%rdx,%rax,4), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = ashr exact i32 %sub, 2
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

define i32* @test_exact4(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact4:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    shrl %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact4:
; X64:       # %bb.0:
; X64-NEXT:    # kill: def $esi killed $esi def $rsi
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    shrl $3, %esi
; X64-NEXT:    leaq (%rdx,%rsi,4), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = lshr exact i32 %sub, 3
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

define i32* @test_exact5(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact5:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    shrl %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact5:
; X64:       # %bb.0:
; X64-NEXT:    # kill: def $esi killed $esi def $rsi
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    shrl $3, %esi
; X64-NEXT:    leaq (%rdx,%rsi,4), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = lshr exact i32 %sub, 3
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

define i32* @test_exact6(i32 %a, i32 %b, i32* %x)  {
; X32-LABEL: test_exact6:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    subl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    addl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    retl
;
; X64-LABEL: test_exact6:
; X64:       # %bb.0:
; X64-NEXT:    # kill: def $esi killed $esi def $rsi
; X64-NEXT:    subl %edi, %esi
; X64-NEXT:    leaq (%rsi,%rdx), %rax
; X64-NEXT:    retq
  %sub = sub i32 %b, %a
  %shr = lshr exact i32 %sub, 2
  %gep = getelementptr inbounds i32, i32* %x, i32 %shr
  ret i32* %gep
}

; PR42644 - https://bugs.llvm.org/show_bug.cgi?id=42644

define i64 @ashr_add_shl_i32(i64 %r) nounwind {
; X32-LABEL: ashr_add_shl_i32:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    incl %eax
; X32-NEXT:    movl %eax, %edx
; X32-NEXT:    sarl $31, %edx
; X32-NEXT:    retl
;
; X64-LABEL: ashr_add_shl_i32:
; X64:       # %bb.0:
; X64-NEXT:    shlq $32, %rdi
; X64-NEXT:    movabsq $4294967296, %rax # imm = 0x100000000
; X64-NEXT:    addq %rdi, %rax
; X64-NEXT:    sarq $32, %rax
; X64-NEXT:    retq
  %conv = shl i64 %r, 32
  %sext = add i64 %conv, 4294967296
  %conv1 = ashr i64 %sext, 32
  ret i64 %conv1
}

define i64 @ashr_add_shl_i8(i64 %r) nounwind {
; X32-LABEL: ashr_add_shl_i8:
; X32:       # %bb.0:
; X32-NEXT:    movsbl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    movl %eax, %edx
; X32-NEXT:    sarl $31, %edx
; X32-NEXT:    retl
;
; X64-LABEL: ashr_add_shl_i8:
; X64:       # %bb.0:
; X64-NEXT:    movsbq %dil, %rax
; X64-NEXT:    retq
  %conv = shl i64 %r, 56
  %sext = add i64 %conv, 4294967296
  %conv1 = ashr i64 %sext, 56
  ret i64 %conv1
}

define <4 x i32> @ashr_add_shl_v4i8(<4 x i32> %r) nounwind {
; X32-LABEL: ashr_add_shl_v4i8:
; X32:       # %bb.0:
; X32-NEXT:    pushl %edi
; X32-NEXT:    pushl %esi
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    movl {{[0-9]+}}(%esp), %ecx
; X32-NEXT:    movl {{[0-9]+}}(%esp), %edx
; X32-NEXT:    movl {{[0-9]+}}(%esp), %esi
; X32-NEXT:    movl {{[0-9]+}}(%esp), %edi
; X32-NEXT:    shll $24, %edi
; X32-NEXT:    shll $24, %esi
; X32-NEXT:    shll $24, %edx
; X32-NEXT:    shll $24, %ecx
; X32-NEXT:    addl $16777216, %ecx # imm = 0x1000000
; X32-NEXT:    addl $16777216, %edx # imm = 0x1000000
; X32-NEXT:    addl $16777216, %esi # imm = 0x1000000
; X32-NEXT:    addl $16777216, %edi # imm = 0x1000000
; X32-NEXT:    sarl $24, %edi
; X32-NEXT:    sarl $24, %esi
; X32-NEXT:    sarl $24, %edx
; X32-NEXT:    sarl $24, %ecx
; X32-NEXT:    movl %ecx, 12(%eax)
; X32-NEXT:    movl %edx, 8(%eax)
; X32-NEXT:    movl %esi, 4(%eax)
; X32-NEXT:    movl %edi, (%eax)
; X32-NEXT:    popl %esi
; X32-NEXT:    popl %edi
; X32-NEXT:    retl $4
;
; X64-LABEL: ashr_add_shl_v4i8:
; X64:       # %bb.0:
; X64-NEXT:    pslld $24, %xmm0
; X64-NEXT:    paddd {{.*}}(%rip), %xmm0
; X64-NEXT:    psrad $24, %xmm0
; X64-NEXT:    retq
  %conv = shl <4 x i32> %r, <i32 24, i32 24, i32 24, i32 24>
  %sext = add <4 x i32> %conv, <i32 16777216, i32 16777216, i32 16777216, i32 16777216>
  %conv1 = ashr <4 x i32> %sext, <i32 24, i32 24, i32 24, i32 24>
  ret <4 x i32> %conv1
}

define i64 @ashr_add_shl_i36(i64 %r) nounwind {
; X32-LABEL: ashr_add_shl_i36:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %edx
; X32-NEXT:    shll $4, %edx
; X32-NEXT:    movl %edx, %eax
; X32-NEXT:    sarl $4, %eax
; X32-NEXT:    sarl $31, %edx
; X32-NEXT:    retl
;
; X64-LABEL: ashr_add_shl_i36:
; X64:       # %bb.0:
; X64-NEXT:    movq %rdi, %rax
; X64-NEXT:    shlq $36, %rax
; X64-NEXT:    sarq $36, %rax
; X64-NEXT:    retq
  %conv = shl i64 %r, 36
  %sext = add i64 %conv, 4294967296
  %conv1 = ashr i64 %sext, 36
  ret i64 %conv1
}

define i64 @ashr_add_shl_mismatch_shifts1(i64 %r) nounwind {
; X32-LABEL: ashr_add_shl_mismatch_shifts1:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    incl %eax
; X32-NEXT:    movl %eax, %edx
; X32-NEXT:    sarl $31, %edx
; X32-NEXT:    retl
;
; X64-LABEL: ashr_add_shl_mismatch_shifts1:
; X64:       # %bb.0:
; X64-NEXT:    shlq $8, %rdi
; X64-NEXT:    movabsq $4294967296, %rax # imm = 0x100000000
; X64-NEXT:    addq %rdi, %rax
; X64-NEXT:    sarq $32, %rax
; X64-NEXT:    retq
  %conv = shl i64 %r, 8
  %sext = add i64 %conv, 4294967296
  %conv1 = ashr i64 %sext, 32
  ret i64 %conv1
}

define i64 @ashr_add_shl_mismatch_shifts2(i64 %r) nounwind {
; X32-LABEL: ashr_add_shl_mismatch_shifts2:
; X32:       # %bb.0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    movl {{[0-9]+}}(%esp), %edx
; X32-NEXT:    shrdl $8, %edx, %eax
; X32-NEXT:    shrl $8, %edx
; X32-NEXT:    incl %edx
; X32-NEXT:    shrdl $8, %edx, %eax
; X32-NEXT:    shrl $8, %edx
; X32-NEXT:    retl
;
; X64-LABEL: ashr_add_shl_mismatch_shifts2:
; X64:       # %bb.0:
; X64-NEXT:    shrq $8, %rdi
; X64-NEXT:    movabsq $4294967296, %rax # imm = 0x100000000
; X64-NEXT:    addq %rdi, %rax
; X64-NEXT:    shrq $8, %rax
; X64-NEXT:    retq
  %conv = lshr i64 %r, 8
  %sext = add i64 %conv, 4294967296
  %conv1 = ashr i64 %sext, 8
  ret i64 %conv1
}
