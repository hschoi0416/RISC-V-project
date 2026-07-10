#ifndef RISCV_H_
#define RISCV_H_

#include <stdint.h>

#define UART_BASE 0x10000000UL

#define UART_THR 0
#define UART_LSR 5

#define LSR_THRE (1 << 5)

#define CLINT_BASE 0x02000000UL
#define CLINT_MTIMECMP (CLINT_BASE + 0x4000)
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

#define TICK_INTERVAL 1000000UL
#define read_csr(reg) ({ unsigned long __v; \
    asm volatile ("csrr %0, " #reg : "=r"(__v)); __v; })
#define write_csr(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#define CLINT_MSIP (CLINT_BASE + 0x0000)

#endif
