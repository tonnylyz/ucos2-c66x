#ifndef UCOS2_C66X_PARTITION_H
#define UCOS2_C66X_PARTITION_H

#include <types.h>
#include <ucos_ii.h>

#define PARTITION_MAX_NUM 8
#define PARTITION_MAX_TASK_NUM 64

typedef struct {
    u8 id;
    u8 xmc_id;
    u8 task_num;
    u8 task_priority_list[PARTITION_MAX_TASK_NUM];
    OS_TCB* task_tcb_list[PARTITION_MAX_TASK_NUM];
} partition_t;

extern partition_t partitions[];

void partition_init();

#endif //UCOS2_C66X_PARTITION_H
