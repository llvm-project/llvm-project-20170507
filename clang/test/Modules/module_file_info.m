
@import DependsOnModule;

// RUN: rm -rf %t
// RUN: %clang_cc1 -w -Wunused -fmodules -fdisable-module-hash -fmodules-cache-path=%t -F %S/Inputs -DBLARG -DWIBBLE=WOBBLE %s
// RUN: %clang_cc1 -module-file-info %t/DependsOnModule.pcm | FileCheck %s

// CHECK: Generated by this Clang:

// CHECK: Module name: DependsOnModule
// CHECK: Module map file: {{.*}}DependsOnModule.framework{{[/\\]}}module.map

// CHECK: Language options:
// CHECK:   C99: Yes
// CHECK:   Objective-C 1: Yes
// CHECK:   modules extension to C: Yes

// CHECK: Target options:
// CHECK:     Triple:
// CHECK:     CPU: 
// CHECK:     ABI: 

// CHECK: Diagnostic options:
// CHECK:   IgnoreWarnings: Yes
// CHECK:   Diagnostic flags:
// CHECK:     -Wunused

// CHECK: Header search options:
// CHECK:   System root [-isysroot=]: '/'
// CHECK:   Use builtin include directories [-nobuiltininc]: Yes
// CHECK:   Use standard system include directories [-nostdinc]: Yes
// CHECK:   Use standard C++ include directories [-nostdinc++]: Yes
// CHECK:   Use libc++ (rather than libstdc++) [-stdlib=]:

// CHECK: Preprocessor options:
// CHECK:   Uses compiler/target-specific predefines [-undef]: Yes
// CHECK:   Uses detailed preprocessing record (for indexing): No
// CHECK:   Predefined macros:
// CHECK:     -DBLARG
// CHECK:     -DWIBBLE=WOBBLE
