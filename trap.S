.section .text
.global trap_entry
.align 4

trap_entry:
    call trap_handler_c
    mret
