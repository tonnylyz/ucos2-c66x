#ifndef UCOS2_C66X_PART_CONF_H
#define UCOS2_C66X_PART_CONF_H

#include <types.h>
#include <def.h>

typedef struct {
    u8 identifier;
    u32 memory_addr;
    u32 memory_size;
    u32 stack_addr;
    u32 stack_size;
    u8 task_num;
    process_attribute_t *task_conf_list;
    u32 slice_num;
    u8 target_core;

    /* APEX */
    u32 period;
    u32 duration;
    u8 critical_level;
    u32 entry_point;
    partition_type_t type;
} partition_conf_t;

#endif //UCOS2_C66X_PART_CONF_H
