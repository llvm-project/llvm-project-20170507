// RUN: %clang_cc1 -emit-llvm-only %s
// PR10290

template<int Flags> struct foo {
  int value = Flags && 0;
};

void test() {
  foo<4> bar;
}

