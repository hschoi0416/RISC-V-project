#include <stdint.h>
#include "task.h"

#define UART_BASE 0x10000000UL

#define UART_THR 0
#define UART_LSR 5

#define LSR_THRE (1 << 5)

#define CLINT_BASE 0x02000000UL
#define CLINT_MTIMECMP (CLINT_BASE + 0x4000)
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

#define TICK_INTERVAL 5000000UL
#define read_csr(reg) ({ unsigned long __v; \
    asm volatile ("csrr %0, " #reg : "=r"(__v)); __v; })
#define write_csr(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#define CLINT_MSIP (CLINT_BASE + 0x0000)

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

void uart_puthex(uint64_t val) {
    uart_puts("0x");
    for (int i = 60; i >= 0; i-= 4) {
        int nib = (val >> i) & 0xf;
        uart_putc(nib < 10 ? '0' + nib : 'a' + nib - 10);
    }
    uart_putc('\n');
}

extern void trap_entry(void);
volatile uint64_t tick_count = 0;

void trap_handler_c(void) {
    unsigned long cause = read_csr(mcause);

    unsigned long code = cause & 0xff;
    int is_interrupt = (cause >> 63) & 1;
   
    if (!is_interrupt)
        return;

    if (code == 7) {
        volatile uint64_t *mtime = (volatile uint64_t*)CLINT_MTIME;
        volatile uint64_t *mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP;
        uart_puts("T mtime="); uart_puthex(*mtime); 
        uart_puts("T cmp_old="); uart_puthex(*mtimecmp);
        
        *mtimecmp = *mtime + TICK_INTERVAL;

        uart_puts("T cmp_new="); uart_puthex(*mtimecmp);
        tick_count++;
        uart_puts("tick ");
        uart_putc('0' + (tick_count % 10));
        uart_putc('\n');

        volatile uint32_t *msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 1;
    }
    else if (code == 3){
        volatile uint32_t *msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 0;
        uart_puts("  [SWITCH]\n");
    }   
}

int first_switch = 1;
uint64_t *trap_dispatch(uint64_t *cur_sp) {
    unsigned long cause = read_csr(mcause);
    unsigned long code = cause & 0xff;
    int is_interrupt = (cause >> 63) & 1;

    if (!is_interrupt)
        return cur_sp;

    if (code == 7) {
        volatile uint64_t* mtime = (volatile uint64_t*)CLINT_MTIME;
        volatile uint64_t* mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP;
        
        *mtimecmp = *mtime + TICK_INTERVAL;

        volatile uint32_t* msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 1;

        return cur_sp;

    }

    else if (code == 3) {
        volatile uint32_t* msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 0;
        
        tasks[current_task].sp = cur_sp;
        current_task = (current_task + 1) % MAX_TASKS;

        return tasks[current_task].sp;
    }
    return cur_sp;
}


void timer_init(void) {
    volatile uint64_t* mtime = (volatile uint64_t*)CLINT_MTIME;
    volatile uint64_t* mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP;

    *mtimecmp = *mtime + TICK_INTERVAL;
    write_csr(mtvec, (unsigned long)trap_entry);
    write_csr(mie, read_csr(mie) | (1 << 7) | (1 << 3));
    //write_csr(mstatus, read_csr(mstatus) | (1 << 3));


}

extern void scheduler_start(uint64_t* first_sp);

int main(void) {
    uart_puts("hello\n");
    task_init(); 
    timer_init();
    uart_puts("scheduler start\n");

    current_task = 0;
    scheduler_start(tasks[0].sp);
    while (1) {
    }
    return 0;
}

 
