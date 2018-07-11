# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=znver1 -instruction-tables < %s | FileCheck %s

andn        %eax, %ebx, %ecx
andn        (%rax), %ebx, %ecx

andn        %rax, %rbx, %rcx
andn        (%rax), %rbx, %rcx

bextr       %eax, %ebx, %ecx
bextr       %eax, (%rbx), %ecx

bextr       %rax, %rbx, %rcx
bextr       %rax, (%rbx), %rcx

blsi        %eax, %ecx
blsi        (%rax), %ecx

blsi        %rax, %rcx
blsi        (%rax), %rcx

blsmsk      %eax, %ecx
blsmsk      (%rax), %ecx

blsmsk      %rax, %rcx
blsmsk      (%rax), %rcx

blsr        %eax, %ecx
blsr        (%rax), %ecx

blsr        %rax, %rcx
blsr        (%rax), %rcx

tzcnt       %eax, %ecx
tzcnt       (%rax), %ecx

tzcnt       %rax, %rcx
tzcnt       (%rax), %rcx

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      1     0.25                        andnl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      5     0.50    *                   andnl	(%rax), %ebx, %ecx
# CHECK-NEXT:  1      1     0.25                        andnq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      5     0.50    *                   andnq	(%rax), %rbx, %rcx
# CHECK-NEXT:  1      1     0.25                        bextrl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      5     0.50    *                   bextrl	%eax, (%rbx), %ecx
# CHECK-NEXT:  1      1     0.25                        bextrq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      5     0.50    *                   bextrq	%rax, (%rbx), %rcx
# CHECK-NEXT:  1      2     0.25                        blsil	%eax, %ecx
# CHECK-NEXT:  1      6     0.50    *                   blsil	(%rax), %ecx
# CHECK-NEXT:  1      2     0.25                        blsiq	%rax, %rcx
# CHECK-NEXT:  1      6     0.50    *                   blsiq	(%rax), %rcx
# CHECK-NEXT:  1      2     0.25                        blsmskl	%eax, %ecx
# CHECK-NEXT:  1      6     0.50    *                   blsmskl	(%rax), %ecx
# CHECK-NEXT:  1      2     0.25                        blsmskq	%rax, %rcx
# CHECK-NEXT:  1      6     0.50    *                   blsmskq	(%rax), %rcx
# CHECK-NEXT:  1      2     0.25                        blsrl	%eax, %ecx
# CHECK-NEXT:  1      6     0.50    *                   blsrl	(%rax), %ecx
# CHECK-NEXT:  1      2     0.25                        blsrq	%rax, %rcx
# CHECK-NEXT:  1      6     0.50    *                   blsrq	(%rax), %rcx
# CHECK-NEXT:  1      2     0.25                        tzcntl	%eax, %ecx
# CHECK-NEXT:  2      6     0.50    *                   tzcntl	(%rax), %ecx
# CHECK-NEXT:  1      2     0.25                        tzcntq	%rax, %rcx
# CHECK-NEXT:  2      6     0.50    *                   tzcntq	(%rax), %rcx

# CHECK:      Resources:
# CHECK-NEXT: [0]   - ZnAGU0
# CHECK-NEXT: [1]   - ZnAGU1
# CHECK-NEXT: [2]   - ZnALU0
# CHECK-NEXT: [3]   - ZnALU1
# CHECK-NEXT: [4]   - ZnALU2
# CHECK-NEXT: [5]   - ZnALU3
# CHECK-NEXT: [6]   - ZnDivider
# CHECK-NEXT: [7]   - ZnFPU0
# CHECK-NEXT: [8]   - ZnFPU1
# CHECK-NEXT: [9]   - ZnFPU2
# CHECK-NEXT: [10]  - ZnFPU3
# CHECK-NEXT: [11]  - ZnMultiplier

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]
# CHECK-NEXT: 6.00   6.00   6.00   6.00   6.00   6.00    -      -      -      -      -      -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   Instructions:
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     andnl	%eax, %ebx, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     andnl	(%rax), %ebx, %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     andnq	%rax, %rbx, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     andnq	(%rax), %rbx, %rcx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     bextrl	%eax, %ebx, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     bextrl	%eax, (%rbx), %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     bextrq	%rax, %rbx, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     bextrq	%rax, (%rbx), %rcx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsil	%eax, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsil	(%rax), %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsiq	%rax, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsiq	(%rax), %rcx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsmskl	%eax, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsmskl	(%rax), %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsmskq	%rax, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsmskq	(%rax), %rcx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsrl	%eax, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsrl	(%rax), %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsrq	%rax, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     blsrq	(%rax), %rcx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     tzcntl	%eax, %ecx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     tzcntl	(%rax), %ecx
# CHECK-NEXT:  -      -     0.25   0.25   0.25   0.25    -      -      -      -      -      -     tzcntq	%rax, %rcx
# CHECK-NEXT: 0.50   0.50   0.25   0.25   0.25   0.25    -      -      -      -      -      -     tzcntq	(%rax), %rcx
