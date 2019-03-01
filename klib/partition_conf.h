#ifndef UCOS2_C66X_PART_CONF_H
#define UCOS2_C66X_PART_CONF_H

#include <types.h>

typedef struct {
    u32 address;
    u32 size;
} memory_conf_t;

typedef struct {
    void (*entry)(void *);
    void *stack_ptr;
    u32 stack_size;
    void *arg;
    u8 priority;
} task_conf_t;

typedef struct {
    memory_conf_t memory_conf;
    u8 task_num;
    task_conf_t *task_conf_list;
    u32 slice_ticks;
    u8 target_core;
} partition_conf_t;

#endif //UCOS2_C66X_PART_CONF_H
