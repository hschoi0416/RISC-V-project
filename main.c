#include <stdint.h>

#define UART_BASE 0x10000000UL

#define UART_THR 0
#define UART_LSR 5

#define LSR_THRE (1 << 5)

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

int main(void) {
    uart_puts("hello\n");
    while (1) {
    }
    return 0;
}

 
