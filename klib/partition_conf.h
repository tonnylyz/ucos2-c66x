#ifndef UCOS2_C66X_PART_CONF_H
#define UCOS2_C66X_PART_CONF_H

#include <types.h>

typedef struct {
    u32 address;
    u32 size;
} memory_layout_t;

typedef struct {
    void (*entry)(void *);
    void *stack_ptr;
    u32 stack_size;
    void *arg;
    u8 priority;
} task_init_t;

typedef struct {
    memory_layout_t memory_layout;
    u8 task_num;
    task_init_t *task_init;
} part_init_t;

#endif //UCOS2_C66X_PART_CONF_H
