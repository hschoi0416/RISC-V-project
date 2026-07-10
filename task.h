#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>

#define MAX_TASKS 3 
#define STACK_SIZE 4096

#define CONTEXT_SIZE 32

typedef enum {
    TASK_READY,
    TASK_BLOCKED
} task_state_t;

typedef struct {
    uint64_t *sp;
    task_state_t state;
    uint64_t wake_tick;
} tcb_t;

extern tcb_t tasks[MAX_TASKS];
extern int current_task;

void task_create(int id, void (*func)(void));
void task_init(void);
void task_sleep(uint64_t ticks);
#endif
