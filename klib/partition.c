#include "partition.h"
#include "partition_conf.h"
#include <ucos_ii.h>
#include <xmc.h>

partition_t partitions[PARTITION_MAX_NUM];

extern part_init_t p0_init;

static u8 usedIndex = 0;

static void partition_add(part_init_t *init) {
    u32 i, r;
    if (usedIndex >= PARTITION_MAX_NUM) {
        panic("partition_add: No available partition");
    }

    partitions[usedIndex].id = usedIndex;
    partitions[usedIndex].task_num = init->task_num;
    for (i = 0; i < init->task_num; i++) {
        r = OSTaskCreate(
                init->task_init[i].entry,
                init->task_init[i].arg,
                init->task_init[i].stack_ptr,
                init->task_init[i].priority);
        if (r != 0) {
            printf("OSTaskCreate return %d\n", r);
            panic("Create task failed");
        }
        partitions->task_priority_list[i] = init->task_init[i].priority;
        partitions->task_tcb_list[i] = OSTCBPrioTbl[init->task_init[i].priority];
    }

    partitions[usedIndex].xmc_id = xmc_segment_allocate(&init->memory_layout);
    usedIndex++;
}

void partition_init() {
    partition_add(&p0_init);
}
