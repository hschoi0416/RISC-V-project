#include <stdint.h>
#include "task.h"
#include "riscv.h"


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
volatile uint64_t global_tick = 0;

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

        global_tick++;

        for (int i = 0; i < MAX_TASKS; i++) {
            if (tasks[i].state == TASK_BLOCKED && global_tick >= tasks[i].wake_tick) {
                tasks[i].state = TASK_READY;
            }
        }

        volatile uint32_t* msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 1;

        return cur_sp;

    }

    else if (code == 3) {
        volatile uint32_t* msip = (volatile uint32_t*)CLINT_MSIP;
        *msip = 0;
        
        tasks[current_task].sp = cur_sp;

        int next = current_task;
        for (int i = 0; i < MAX_TASKS; i++) {
            next = (next + 1) % MAX_TASKS;
            if (tasks[next].state == TASK_READY)
                break;
        }
        current_task = next;

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

 
