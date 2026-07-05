#include "task.h"

tcb_t tasks[MAX_TASKS];

int current_task = 0;

static uint64_t task_stacks[MAX_TASKS][STACK_SIZE / 8] __attribute__((aligned(16)));

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
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void task_b(void) {
    while(1) {
    
        uart_puts("   <Task B>\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void task_init(void) {
    task_create(0, task_a);
    task_create(1, task_b);
    current_task = 0;

}
