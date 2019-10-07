; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -march=mips64 -relocation-model=static < %s \
; RUN:   | FileCheck %s --check-prefix STATIC
; RUN: llc -march=mips64 -relocation-model=pic < %s \
; RUN:   | FileCheck %s --check-prefix PIC

declare dso_local void @callee() noreturn nounwind

define dso_local void @caller() nounwind "no-frame-pointer-elim-non-leaf" {
; STATIC-LABEL: caller:
; STATIC:       # %bb.0: # %entry
; STATIC-NEXT:    daddiu $sp, $sp, -16
; STATIC-NEXT:    sd $ra, 8($sp) # 8-byte Folded Spill
; STATIC-NEXT:    sd $fp, 0($sp) # 8-byte Folded Spill
; STATIC-NEXT:    move $fp, $sp
; STATIC-NEXT:    jal callee
; STATIC-NEXT:    nop
;
; PIC-LABEL: caller:
; PIC:       # %bb.0: # %entry
; PIC-NEXT:    daddiu $sp, $sp, -32
; PIC-NEXT:    sd $ra, 24($sp) # 8-byte Folded Spill
; PIC-NEXT:    sd $fp, 16($sp) # 8-byte Folded Spill
; PIC-NEXT:    sd $gp, 8($sp) # 8-byte Folded Spill
; PIC-NEXT:    move $fp, $sp
; PIC-NEXT:    lui $1, %hi(%neg(%gp_rel(caller)))
; PIC-NEXT:    daddu $1, $1, $25
; PIC-NEXT:    daddiu $gp, $1, %lo(%neg(%gp_rel(caller)))
; PIC-NEXT:    ld $25, %call16(callee)($gp)
; PIC-NEXT:    .reloc .Ltmp0, R_MIPS_JALR, callee
; PIC-NEXT:  .Ltmp0:
; PIC-NEXT:    jalr $25
; PIC-NEXT:    nop
entry:
  tail call void @callee()
  unreachable
}
