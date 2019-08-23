// RUN: %check_clang_tidy %s cert-err09-cpp,cert-err61-cpp %t

void alwaysThrows() {
  int ex = 42;
  // CHECK-MESSAGES: warning: throw expression should throw anonymous temporary values instead [cert-err09-cpp]
  // CHECK-MESSAGES: warning: throw expression should throw anonymous temporary values instead [cert-err61-cpp]
  throw ex;
}

void doTheJob() {
  try {
    alwaysThrows();
  } catch (int&) {
  }
}
