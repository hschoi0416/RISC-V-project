#include "task.h"
#include "riscv.h"

tcb_t tasks[MAX_TASKS];

int current_task = 0;
extern volatile uint64_t global_tick;

static uint64_t task_stacks[MAX_TASKS][STACK_SIZE / 8] __attribute__((aligned(16)));
void task_sleep(uint64_t ticks);
extern int uart_puthex(uint64_t val);
void task_create(int id, void (*func)(void)) {
    uint64_t *stack_top = &task_stacks[id][STACK_SIZE / 8];
    uint64_t *sp = stack_top - CONTEXT_SIZE;

    for (int i = 0; i < CONTEXT_SIZE; i++)
        sp[i] = 0;

    sp[0] = (uint64_t)func;

    sp[31] = (3UL << 11) | (1UL <<7);

    tasks[id].sp = sp;
}

extern void uart_puts(const char* s);

void task_a(void){
    while(1) {
        uart_puts("  <Task A>\n");
        task_sleep(50); 
    }
}

void task_b(void) {
    while(1) {
        uart_puts("   <Task B>\n");
        task_sleep(30); 
    }
}

void task_idle(void) {
    while(1) {
        asm volatile ("wfi");
    }
}


void task_init(void) {
    task_create(0, task_a);
    task_create(1, task_b);
    task_create(2, task_idle);

    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_READY;
    }

    current_task = 0;

}

void task_sleep(uint64_t ticks) {
    write_csr(mstatus, read_csr(mstatus) & ~(1UL << 3));

    tasks[current_task].wake_tick = global_tick + ticks;
    tasks[current_task].state = TASK_BLOCKED;

    write_csr(mstatus, read_csr(mstatus) | (1UL << 3));

    volatile uint32_t *msip = (volatile uint32_t* )CLINT_MSIP;
    *msip = 1;

    while (tasks[current_task].state == TASK_BLOCKED);
}
