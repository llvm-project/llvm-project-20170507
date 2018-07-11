# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=x86-64 -instruction-tables < %s | FileCheck %s

bzhi        %eax, %ebx, %ecx
bzhi        %eax, (%rbx), %ecx

bzhi        %rax, %rbx, %rcx
bzhi        %rax, (%rbx), %rcx

mulx        %eax, %ebx, %ecx
mulx        (%rax), %ebx, %ecx

mulx        %rax, %rbx, %rcx
mulx        (%rax), %rbx, %rcx

pdep        %eax, %ebx, %ecx
pdep        (%rax), %ebx, %ecx

pdep        %rax, %rbx, %rcx
pdep        (%rax), %rbx, %rcx

pext        %eax, %ebx, %ecx
pext        (%rax), %ebx, %ecx

pext        %rax, %rbx, %rcx
pext        (%rax), %rbx, %rcx

rorx        $1, %eax, %ecx
rorx        $1, (%rax), %ecx

rorx        $1, %rax, %rcx
rorx        $1, (%rax), %rcx

sarx        %eax, %ebx, %ecx
sarx        %eax, (%rbx), %ecx

sarx        %rax, %rbx, %rcx
sarx        %rax, (%rbx), %rcx

shlx        %eax, %ebx, %ecx
shlx        %eax, (%rbx), %ecx

shlx        %rax, %rbx, %rcx
shlx        %rax, (%rbx), %rcx

shrx        %eax, %ebx, %ecx
shrx        %eax, (%rbx), %ecx

shrx        %rax, %rbx, %rcx
shrx        %rax, (%rbx), %rcx

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      1     1.00                        bzhil	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     1.00    *                   bzhil	%eax, (%rbx), %ecx
# CHECK-NEXT:  1      1     1.00                        bzhiq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     1.00    *                   bzhiq	%rax, (%rbx), %rcx
# CHECK-NEXT:  2      3     1.00                        mulxl	%eax, %ebx, %ecx
# CHECK-NEXT:  3      8     1.00    *                   mulxl	(%rax), %ebx, %ecx
# CHECK-NEXT:  2      3     1.00                        mulxq	%rax, %rbx, %rcx
# CHECK-NEXT:  3      8     1.00    *                   mulxq	(%rax), %rbx, %rcx
# CHECK-NEXT:  1      1     0.33                        pdepl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     0.50    *                   pdepl	(%rax), %ebx, %ecx
# CHECK-NEXT:  1      1     0.33                        pdepq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     0.50    *                   pdepq	(%rax), %rbx, %rcx
# CHECK-NEXT:  1      1     0.33                        pextl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     0.50    *                   pextl	(%rax), %ebx, %ecx
# CHECK-NEXT:  1      1     0.33                        pextq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     0.50    *                   pextq	(%rax), %rbx, %rcx
# CHECK-NEXT:  1      1     0.50                        rorxl	$1, %eax, %ecx
# CHECK-NEXT:  2      6     0.50    *                   rorxl	$1, (%rax), %ecx
# CHECK-NEXT:  1      1     0.50                        rorxq	$1, %rax, %rcx
# CHECK-NEXT:  2      6     0.50    *                   rorxq	$1, (%rax), %rcx
# CHECK-NEXT:  1      1     0.50                        sarxl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     0.50    *                   sarxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  1      1     0.50                        sarxq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     0.50    *                   sarxq	%rax, (%rbx), %rcx
# CHECK-NEXT:  1      1     0.50                        shlxl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     0.50    *                   shlxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  1      1     0.50                        shlxq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     0.50    *                   shlxq	%rax, (%rbx), %rcx
# CHECK-NEXT:  1      1     0.50                        shrxl	%eax, %ebx, %ecx
# CHECK-NEXT:  2      6     0.50    *                   shrxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  1      1     0.50                        shrxq	%rax, %rbx, %rcx
# CHECK-NEXT:  2      6     0.50    *                   shrxq	%rax, (%rbx), %rcx

# CHECK:      Resources:
# CHECK-NEXT: [0]   - SBDivider
# CHECK-NEXT: [1]   - SBFPDivider
# CHECK-NEXT: [2]   - SBPort0
# CHECK-NEXT: [3]   - SBPort1
# CHECK-NEXT: [4]   - SBPort4
# CHECK-NEXT: [5]   - SBPort5
# CHECK-NEXT: [6.0] - SBPort23
# CHECK-NEXT: [6.1] - SBPort23

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6.0]  [6.1]
# CHECK-NEXT:  -      -     10.67  10.67   -     10.67  8.00   8.00

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6.0]  [6.1]  Instructions:
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -     bzhil	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -     0.50   0.50   bzhil	%eax, (%rbx), %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -     bzhiq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -      -     1.00    -      -     0.50   0.50   bzhiq	%rax, (%rbx), %rcx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -     mulxl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -     0.50   0.50   mulxl	(%rax), %ebx, %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -     mulxq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -      -     1.00    -      -     0.50   0.50   mulxq	(%rax), %rbx, %rcx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33    -      -     pdepl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33   0.50   0.50   pdepl	(%rax), %ebx, %ecx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33    -      -     pdepq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33   0.50   0.50   pdepq	(%rax), %rbx, %rcx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33    -      -     pextl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33   0.50   0.50   pextl	(%rax), %ebx, %ecx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33    -      -     pextq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -     0.33   0.33    -     0.33   0.50   0.50   pextq	(%rax), %rbx, %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     rorxl	$1, %eax, %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   rorxl	$1, (%rax), %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     rorxq	$1, %rax, %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   rorxq	$1, (%rax), %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     sarxl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   sarxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     sarxq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   sarxq	%rax, (%rbx), %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     shlxl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   shlxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     shlxq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   shlxq	%rax, (%rbx), %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     shrxl	%eax, %ebx, %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   shrxl	%eax, (%rbx), %ecx
# CHECK-NEXT:  -      -     0.50    -      -     0.50    -      -     shrxq	%rax, %rbx, %rcx
# CHECK-NEXT:  -      -     0.50    -      -     0.50   0.50   0.50   shrxq	%rax, (%rbx), %rcx
