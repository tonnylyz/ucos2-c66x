#ifndef UCOS2_C66X_PART_CONF_H
#define UCOS2_C66X_PART_CONF_H

#include <types.h>
#include <def.h>

typedef struct {
    u32 address;
    u32 size;
} memory_conf_t;

typedef struct {
    char name[APEX_NAME_MAX_LEN];
    void (*entry)(void *);
    u32 stack_size;
    void *arg;
    u8 priority;
} task_conf_t;

typedef struct {

} communication_conf_t;

typedef struct {
    u8 identifier;
    memory_conf_t memory_conf;
    u32 stack_addr;
    u32 stack_size;

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
