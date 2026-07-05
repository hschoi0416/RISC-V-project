#include <stdint.h>

#define UART_BASE 0x10000000UL

#define UART_THR 0
#define UART_LSR 5

#define LSR_THRE (1 << 5)

#define CLINT_BASE 0x02000000UL
#define CLINT_MTIMECMP (CLINT_BASE + 0x4000)
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

#define TICK_INTERVAL 10000000UL
#define read_csr(reg) ({ unsigned long __v; \
    asm volatile ("csrr %0, " #reg : "=r"(__v)); __v; })
#define write_csr(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })



static inline volatile uint8_t* uart_reg(int off) {
    return (volatile uint8_t*)(UART_BASE + off);
}

void uart_putc(char c) {
    while ((*uart_reg(UART_LSR) & LSR_THRE) == 0);
    *uart_reg(UART_THR) = (uint8_t)c;
}

void uart_puts(const char* s) {
    while(*s) {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

extern void trap_entry(void);
volatile uint64_t tick_count = 0;

void trap_handler_c(void) {
    unsigned long cause = read_csr(mcause);

    unsigned long code = cause & 0xff;
    int is_interrupt = (cause >> 63) & 1;
    
    if (is_interrupt && code == 7) {
        volatile uint64_t *mtime = (volatile uint64_t*)CLINT_MTIME;
        volatile uint64_t *mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP;
        *mtimecmp = *mtime + TICK_INTERVAL;

        tick_count++;
        uart_puts("tick ");
        uart_putc('0' + (tick_count % 10));
        uart_putc('\n');
    }
}

void timer_init(void) {
    volatile uint64_t* mtime = (volatile uint64_t*)CLINT_MTIME;
    volatile uint64_t* mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP;

    *mtimecmp = *mtime + TICK_INTERVAL;
    write_csr(mtvec, (unsigned long)trap_entry);
    write_csr(mie, read_csr(mie) | (1 << 7));
    write_csr(mstatus, read_csr(mstatus) | (1 << 3));


}


int main(void) {
    uart_puts("hello\n");
    timer_init();
    uart_puts("timer armed\n");

    while (1) {
    }
    return 0;
}

 
