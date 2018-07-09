// RUN: rm -rf %t
// RUN: mkdir -p %t
// RUN: not %clang -fcrash-diagnostics-dir=%t -c %s 2>&1 | FileCheck %s
#pragma clang __debug parser_crash
// CHECK: Preprocessed source(s) and associated run script(s) are located at:
// CHECK: diagnostic msg: {{.*}}Output{{/|\\}}crash-diagnostics-dir.c.tmp{{(/|\\).*}}.c
