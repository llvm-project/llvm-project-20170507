# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=skylake-avx512 -instruction-tables < %s | FileCheck %s

vaddpd            %zmm16, %zmm17, %zmm19
vaddpd            (%rax), %zmm17, %zmm19
vaddpd            (%rax){1to8}, %zmm17, %zmm19
vaddpd            %zmm16, %zmm17, %zmm19 {k1}
vaddpd            (%rax), %zmm17, %zmm19 {k1}
vaddpd            (%rax){1to8}, %zmm17, %zmm19 {k1}
vaddpd            %zmm16, %zmm17, %zmm19 {z}{k1}
vaddpd            (%rax), %zmm17, %zmm19 {z}{k1}
vaddpd            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vaddps            %zmm16, %zmm17, %zmm19
vaddps            (%rax), %zmm17, %zmm19
vaddps            (%rax){1to16}, %zmm17, %zmm19
vaddps            %zmm16, %zmm17, %zmm19 {k1}
vaddps            (%rax), %zmm17, %zmm19 {k1}
vaddps            (%rax){1to16}, %zmm17, %zmm19 {k1}
vaddps            %zmm16, %zmm17, %zmm19 {z}{k1}
vaddps            (%rax), %zmm17, %zmm19 {z}{k1}
vaddps            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

vdivpd            %zmm16, %zmm17, %zmm19
vdivpd            (%rax), %zmm17, %zmm19
vdivpd            (%rax){1to8}, %zmm17, %zmm19
vdivpd            %zmm16, %zmm17, %zmm19 {k1}
vdivpd            (%rax), %zmm17, %zmm19 {k1}
vdivpd            (%rax){1to8}, %zmm17, %zmm19 {k1}
vdivpd            %zmm16, %zmm17, %zmm19 {z}{k1}
vdivpd            (%rax), %zmm17, %zmm19 {z}{k1}
vdivpd            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vdivps            %zmm16, %zmm17, %zmm19
vdivps            (%rax), %zmm17, %zmm19
vdivps            (%rax){1to16}, %zmm17, %zmm19
vdivps            %zmm16, %zmm17, %zmm19 {k1}
vdivps            (%rax), %zmm17, %zmm19 {k1}
vdivps            (%rax){1to16}, %zmm17, %zmm19 {k1}
vdivps            %zmm16, %zmm17, %zmm19 {z}{k1}
vdivps            (%rax), %zmm17, %zmm19 {z}{k1}
vdivps            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

vmulpd            %zmm16, %zmm17, %zmm19
vmulpd            (%rax), %zmm17, %zmm19
vmulpd            (%rax){1to8}, %zmm17, %zmm19
vmulpd            %zmm16, %zmm17, %zmm19 {k1}
vmulpd            (%rax), %zmm17, %zmm19 {k1}
vmulpd            (%rax){1to8}, %zmm17, %zmm19 {k1}
vmulpd            %zmm16, %zmm17, %zmm19 {z}{k1}
vmulpd            (%rax), %zmm17, %zmm19 {z}{k1}
vmulpd            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vmulps            %zmm16, %zmm17, %zmm19
vmulps            (%rax), %zmm17, %zmm19
vmulps            (%rax){1to16}, %zmm17, %zmm19
vmulps            %zmm16, %zmm17, %zmm19 {k1}
vmulps            (%rax), %zmm17, %zmm19 {k1}
vmulps            (%rax){1to16}, %zmm17, %zmm19 {k1}
vmulps            %zmm16, %zmm17, %zmm19 {z}{k1}
vmulps            (%rax), %zmm17, %zmm19 {z}{k1}
vmulps            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

vpabsd            %zmm16, %zmm19
vpabsd            (%rax), %zmm19
vpabsd            (%rax){1to16}, %zmm19
vpabsd            %zmm16, %zmm19 {k1}
vpabsd            (%rax), %zmm19 {k1}
vpabsd            (%rax){1to16}, %zmm19 {k1}
vpabsd            %zmm16, %zmm19 {z}{k1}
vpabsd            (%rax), %zmm19 {z}{k1}
vpabsd            (%rax){1to16}, %zmm19 {z}{k1}

vpabsq            %zmm16, %zmm19
vpabsq            (%rax), %zmm19
vpabsq            (%rax){1to8}, %zmm19
vpabsq            %zmm16, %zmm19 {k1}
vpabsq            (%rax), %zmm19 {k1}
vpabsq            (%rax){1to8}, %zmm19 {k1}
vpabsq            %zmm16, %zmm19 {z}{k1}
vpabsq            (%rax), %zmm19 {z}{k1}
vpabsq            (%rax){1to8}, %zmm19 {z}{k1}

vpaddd            %zmm16, %zmm17, %zmm19
vpaddd            (%rax), %zmm17, %zmm19
vpaddd            (%rax){1to16}, %zmm17, %zmm19
vpaddd            %zmm16, %zmm17, %zmm19 {k1}
vpaddd            (%rax), %zmm17, %zmm19 {k1}
vpaddd            (%rax){1to16}, %zmm17, %zmm19 {k1}
vpaddd            %zmm16, %zmm17, %zmm19 {z}{k1}
vpaddd            (%rax), %zmm17, %zmm19 {z}{k1}
vpaddd            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

vpaddq            %zmm16, %zmm17, %zmm19
vpaddq            (%rax), %zmm17, %zmm19
vpaddq            (%rax){1to8}, %zmm17, %zmm19
vpaddq            %zmm16, %zmm17, %zmm19 {k1}
vpaddq            (%rax), %zmm17, %zmm19 {k1}
vpaddq            (%rax){1to8}, %zmm17, %zmm19 {k1}
vpaddq            %zmm16, %zmm17, %zmm19 {z}{k1}
vpaddq            (%rax), %zmm17, %zmm19 {z}{k1}
vpaddq            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vpsubd            %zmm16, %zmm17, %zmm19
vpsubd            (%rax), %zmm17, %zmm19
vpsubd            (%rax){1to16}, %zmm17, %zmm19
vpsubd            %zmm16, %zmm17, %zmm19 {k1}
vpsubd            (%rax), %zmm17, %zmm19 {k1}
vpsubd            (%rax){1to16}, %zmm17, %zmm19 {k1}
vpsubd            %zmm16, %zmm17, %zmm19 {z}{k1}
vpsubd            (%rax), %zmm17, %zmm19 {z}{k1}
vpsubd            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

vpsubq            %zmm16, %zmm17, %zmm19
vpsubq            (%rax), %zmm17, %zmm19
vpsubq            (%rax){1to8}, %zmm17, %zmm19
vpsubq            %zmm16, %zmm17, %zmm19 {k1}
vpsubq            (%rax), %zmm17, %zmm19 {k1}
vpsubq            (%rax){1to8}, %zmm17, %zmm19 {k1}
vpsubq            %zmm16, %zmm17, %zmm19 {z}{k1}
vpsubq            (%rax), %zmm17, %zmm19 {z}{k1}
vpsubq            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vsubpd            %zmm16, %zmm17, %zmm19
vsubpd            (%rax), %zmm17, %zmm19
vsubpd            (%rax){1to8}, %zmm17, %zmm19
vsubpd            %zmm16, %zmm17, %zmm19 {k1}
vsubpd            (%rax), %zmm17, %zmm19 {k1}
vsubpd            (%rax){1to8}, %zmm17, %zmm19 {k1}
vsubpd            %zmm16, %zmm17, %zmm19 {z}{k1}
vsubpd            (%rax), %zmm17, %zmm19 {z}{k1}
vsubpd            (%rax){1to8}, %zmm17, %zmm19 {z}{k1}

vsubps            %zmm16, %zmm17, %zmm19
vsubps            (%rax), %zmm17, %zmm19
vsubps            (%rax){1to16}, %zmm17, %zmm19
vsubps            %zmm16, %zmm17, %zmm19 {k1}
vsubps            (%rax), %zmm17, %zmm19 {k1}
vsubps            (%rax){1to16}, %zmm17, %zmm19 {k1}
vsubps            %zmm16, %zmm17, %zmm19 {z}{k1}
vsubps            (%rax), %zmm17, %zmm19 {z}{k1}
vsubps            (%rax){1to16}, %zmm17, %zmm19 {z}{k1}

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      4     0.50                        vaddpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vaddpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vaddpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vaddpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      4     0.50                        vaddps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vaddps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vaddps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vaddps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  3      23    16.00                       vdivpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  3      23    16.00                       vdivpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  3      23    16.00                       vdivpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  4      30    16.00   *                   vdivpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  3      18    10.00                       vdivps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  3      18    10.00                       vdivps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  3      18    10.00                       vdivps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  4      25    10.00   *                   vdivps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      4     0.50                        vmulpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vmulpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vmulpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vmulpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      4     0.50                        vmulps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vmulps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vmulps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vmulps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     1.00                        vpabsd	%zmm16, %zmm19
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax), %zmm19
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax){1to16}, %zmm19
# CHECK-NEXT:  1      1     1.00                        vpabsd	%zmm16, %zmm19 {%k1}
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax), %zmm19 {%k1}
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax){1to16}, %zmm19 {%k1}
# CHECK-NEXT:  1      1     1.00                        vpabsd	%zmm16, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax), %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     1.00    *                   vpabsd	(%rax){1to16}, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     1.00                        vpabsq	%zmm16, %zmm19
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax), %zmm19
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax){1to8}, %zmm19
# CHECK-NEXT:  1      1     1.00                        vpabsq	%zmm16, %zmm19 {%k1}
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax), %zmm19 {%k1}
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax){1to8}, %zmm19 {%k1}
# CHECK-NEXT:  1      1     1.00                        vpabsq	%zmm16, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax), %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     1.00    *                   vpabsq	(%rax){1to8}, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     0.33                        vpaddd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  1      1     0.33                        vpaddd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      1     0.33                        vpaddd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpaddd	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     0.33                        vpaddq	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  1      1     0.33                        vpaddq	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      1     0.33                        vpaddq	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpaddq	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     0.33                        vpsubd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  1      1     0.33                        vpsubd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      1     0.33                        vpsubd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpsubd	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      1     0.33                        vpsubq	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  1      1     0.33                        vpsubq	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      1     0.33                        vpsubq	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      8     0.50    *                   vpsubq	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      4     0.50                        vsubpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vsubpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vsubpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vsubpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  1      4     0.50                        vsubps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  1      4     0.50                        vsubps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  1      4     0.50                        vsubps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  2      11    0.50    *                   vsubps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}

# CHECK:      Resources:
# CHECK-NEXT: [0]   - SKXDivider
# CHECK-NEXT: [1]   - SKXFPDivider
# CHECK-NEXT: [2]   - SKXPort0
# CHECK-NEXT: [3]   - SKXPort1
# CHECK-NEXT: [4]   - SKXPort2
# CHECK-NEXT: [5]   - SKXPort3
# CHECK-NEXT: [6]   - SKXPort4
# CHECK-NEXT: [7]   - SKXPort5
# CHECK-NEXT: [8]   - SKXPort6
# CHECK-NEXT: [9]   - SKXPort7

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]
# CHECK-NEXT:  -     234.00 93.00  12.00  42.00  42.00   -     57.00   -      -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    Instructions:
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vaddps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vaddps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     16.00  2.00    -      -      -      -     1.00    -      -     vdivpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -     16.00  2.00    -      -      -      -     1.00    -      -     vdivpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     16.00  2.00    -      -      -      -     1.00    -      -     vdivpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     16.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     10.00  2.00    -      -      -      -     1.00    -      -     vdivps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -     10.00  2.00    -      -      -      -     1.00    -      -     vdivps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -     10.00  2.00    -      -      -      -     1.00    -      -     vdivps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -     10.00  2.00    -     0.50   0.50    -     1.00    -      -     vdivps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vmulps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vmulps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsd	%zmm16, %zmm19
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax), %zmm19
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax){1to16}, %zmm19
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsd	%zmm16, %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax), %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax){1to16}, %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsd	%zmm16, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax), %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsd	(%rax){1to16}, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsq	%zmm16, %zmm19
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax), %zmm19
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax){1to8}, %zmm19
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsq	%zmm16, %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax), %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax){1to8}, %zmm19 {%k1}
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     vpabsq	%zmm16, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax), %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     vpabsq	(%rax){1to8}, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddd	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddq	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddq	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpaddq	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpaddq	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubd	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubq	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubq	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     0.33    -      -     vpsubq	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     0.33    -      -     vpsubq	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubpd	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax){1to8}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubpd	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax){1to8}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubpd	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubpd	(%rax){1to8}, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubps	%zmm16, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax), %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax){1to16}, %zmm17, %zmm19
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubps	%zmm16, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax), %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax){1to16}, %zmm17, %zmm19 {%k1}
# CHECK-NEXT:  -      -     0.50    -      -      -      -     0.50    -      -     vsubps	%zmm16, %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax), %zmm17, %zmm19 {%k1} {z}
# CHECK-NEXT:  -      -     0.50    -     0.50   0.50    -     0.50    -      -     vsubps	(%rax){1to16}, %zmm17, %zmm19 {%k1} {z}
