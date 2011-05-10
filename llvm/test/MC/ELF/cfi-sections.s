// RUN: llvm-mc -filetype=obj -triple x86_64-pc-linux-gnu %s -o - | elf-dump  --dump-section-data | FileCheck -check-prefix=ELF_64 %s
// RUN: llvm-mc -filetype=obj -triple i686-pc-linux-gnu %s -o - | elf-dump  --dump-section-data | FileCheck -check-prefix=ELF_32 %s


// The only difference in gas' output in this test is that we don't produce
// the relocations to .debug_frame (we know their values).

.cfi_sections .debug_frame

f1:
        .cfi_startproc
        nop
        .cfi_endproc

f2:
        .cfi_startproc
        nop
        .cfi_endproc

// ELF_64:      (('sh_name', 0x00000011) # '.debug_frame'
// ELF_64-NEXT:  ('sh_type', 0x00000001)
// ELF_64-NEXT:  ('sh_flags', 0x00000000)
// ELF_64-NEXT:  ('sh_addr', 0x00000000)
// ELF_64-NEXT:  ('sh_offset', 0x00000048)
// ELF_64-NEXT:  ('sh_size', 0x00000048)
// ELF_64-NEXT:  ('sh_link', 0x00000000)
// ELF_64-NEXT:  ('sh_info', 0x00000000)
// ELF_64-NEXT:  ('sh_addralign', 0x00000008)
// ELF_64-NEXT:  ('sh_entsize', 0x00000000)
// ELF_64-NEXT:  ('_section_data', '14000000 ffffffff 01000178 100c0708 90010000 00000000 14000000 1c000000 00000000 00000000 01000000 00000000 14000000 34000000 00000000 00000000 01000000 00000000')

// ELF_32:      (('sh_name', 0x00000010) # '.debug_frame'
// ELF_32-NEXT:  ('sh_type', 0x00000001)
// ELF_32-NEXT:  ('sh_flags', 0x00000000)
// ELF_32-NEXT:  ('sh_addr', 0x00000000)
// ELF_32-NEXT:  ('sh_offset', 0x00000038)
// ELF_32-NEXT:  ('sh_size', 0x00000034)
// ELF_32-NEXT:  ('sh_link', 0x00000000)
// ELF_32-NEXT:  ('sh_info', 0x00000000)
// ELF_32-NEXT:  ('sh_addralign', 0x00000004)
// ELF_32-NEXT:  ('sh_entsize', 0x00000000)
// ELF_32-NEXT:  ('_section_data', '10000000 ffffffff 0100017c 080c0404 88010000 0c000000 18000000 00000000 01000000 0c000000 28000000 01000000 01000000')
