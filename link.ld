OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    RAM (rwxa) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text.init)        /* _start가 맨 앞 = 0x80000000 보장 */
        *(.text*)
    } > RAM

    .rodata : { *(.rodata*) } > RAM
    .data   : { *(.data*)   } > RAM

    .bss : {
        _bss_start = .;      /* Startup.S가 참조 */
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    . = ALIGN(16);
    . = . + 0x4000;          /* 16KB 스택 */
    _stack_top = .;          /* Startup.S가 참조 */
}
