// RUN: %clang_cc1 -triple x86_64-linux-gnu -fsyntax-only -ast-dump=json %s | FileCheck %s

unsigned char implicitcast_0(unsigned int x) {
  return x;
}

signed char implicitcast_1(unsigned int x) {
  return x;
}

unsigned char implicitcast_2(signed int x) {
  return x;
}

signed char implicitcast_3(signed int x) {
  return x;
}

//----------------------------------------------------------------------------//

unsigned char cstylecast_0(unsigned int x) {
  return (unsigned char)x;
}

signed char cstylecast_1(unsigned int x) {
  return (signed char)x;
}

unsigned char cstylecast_2(signed int x) {
  return (unsigned char)x;
}

signed char cstylecast_3(signed int x) {
  return (signed char)x;
}

// NOTE: CHECK lines have been autogenerated by gen_ast_dump_json_test.py
// using --filters=ImplicitCastExpr,CStyleCastExpr


// CHECK:  "kind": "ImplicitCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "unsigned char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "unsigned int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "unsigned int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "unsigned int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "ImplicitCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "signed char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "unsigned int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "unsigned int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "unsigned int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "ImplicitCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "unsigned char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "ImplicitCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "signed char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 10,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 10,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "CStyleCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 25,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "unsigned char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 25,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 25,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "unsigned int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "isPartOfExplicitCast": true,
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 25,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 25,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "unsigned int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "unsigned int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "CStyleCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 23,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "signed char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 23,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 23,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "unsigned int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "isPartOfExplicitCast": true,
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 23,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 23,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "unsigned int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "unsigned int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "CStyleCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 25,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "unsigned char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 25,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 25,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "isPartOfExplicitCast": true,
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 25,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 25,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }


// CHECK:  "kind": "CStyleCastExpr",
// CHECK-NEXT:  "range": {
// CHECK-NEXT:   "begin": {
// CHECK-NEXT:    "col": 10,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   },
// CHECK-NEXT:   "end": {
// CHECK-NEXT:    "col": 23,
// CHECK-NEXT:    "tokLen": 1
// CHECK-NEXT:   }
// CHECK-NEXT:  },
// CHECK-NEXT:  "type": {
// CHECK-NEXT:   "qualType": "signed char"
// CHECK-NEXT:  },
// CHECK-NEXT:  "valueCategory": "rvalue",
// CHECK-NEXT:  "castKind": "IntegralCast",
// CHECK-NEXT:  "inner": [
// CHECK-NEXT:   {
// CHECK-NEXT:    "id": "0x{{.*}}",
// CHECK-NEXT:    "kind": "ImplicitCastExpr",
// CHECK-NEXT:    "range": {
// CHECK-NEXT:     "begin": {
// CHECK-NEXT:      "col": 23,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     },
// CHECK-NEXT:     "end": {
// CHECK-NEXT:      "col": 23,
// CHECK-NEXT:      "tokLen": 1
// CHECK-NEXT:     }
// CHECK-NEXT:    },
// CHECK-NEXT:    "type": {
// CHECK-NEXT:     "qualType": "int"
// CHECK-NEXT:    },
// CHECK-NEXT:    "valueCategory": "rvalue",
// CHECK-NEXT:    "castKind": "LValueToRValue",
// CHECK-NEXT:    "isPartOfExplicitCast": true,
// CHECK-NEXT:    "inner": [
// CHECK-NEXT:     {
// CHECK-NEXT:      "id": "0x{{.*}}",
// CHECK-NEXT:      "kind": "DeclRefExpr",
// CHECK-NEXT:      "range": {
// CHECK-NEXT:       "begin": {
// CHECK-NEXT:        "col": 23,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       },
// CHECK-NEXT:       "end": {
// CHECK-NEXT:        "col": 23,
// CHECK-NEXT:        "tokLen": 1
// CHECK-NEXT:       }
// CHECK-NEXT:      },
// CHECK-NEXT:      "type": {
// CHECK-NEXT:       "qualType": "int"
// CHECK-NEXT:      },
// CHECK-NEXT:      "valueCategory": "lvalue",
// CHECK-NEXT:      "referencedDecl": {
// CHECK-NEXT:       "id": "0x{{.*}}",
// CHECK-NEXT:       "kind": "ParmVarDecl",
// CHECK-NEXT:       "name": "x",
// CHECK-NEXT:       "type": {
// CHECK-NEXT:        "qualType": "int"
// CHECK-NEXT:       }
// CHECK-NEXT:      }
// CHECK-NEXT:     }
// CHECK-NEXT:    ]
// CHECK-NEXT:   }
// CHECK-NEXT:  ]
// CHECK-NEXT: }
