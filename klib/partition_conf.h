#ifndef UCOS2_C66X_PART_CONF_H
#define UCOS2_C66X_PART_CONF_H

#include <types.h>
#include <def.h>

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
    u8 dest_num;
    u8 *dest_list;
    u8 src_num;
    u8 *src_list;
} communication_conf_t;

typedef struct {
    u8 identifier;
    memory_conf_t memory_conf;

    /* APEX compliance */
    u32 period;
    u32 duration;
    u8 critical_level;
    communication_conf_t communication_conf;
    /* Health Monitor not implemented */
    u32 entry_point;
    partition_type_t type;

    u8 task_num;
    task_conf_t *task_conf_list;

    u32 slice_ticks;
    u8 target_core;

} partition_conf_t;

#endif //UCOS2_C66X_PART_CONF_H
