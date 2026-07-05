#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>

#define MAX_TASKS 2
#define STACK_SIZE 4096

#define CONTEXT_SIZE 32

typedef struct {
    uint64_t *sp;
} tcb_t;

extern tcb_t tasks[MAX_TASKS];
extern int current_task;

void task_create(int id, void (*func)(void));
void task_init(void);

#endif
