#include "partition.h"

#include <printf.h>
#include <ucos_ii.h>
#include <xmc.h>
#include <timer.h>
#include <intc.h>

pcb_t *partition_current;

#pragma DATA_SECTION(pcb_list, ".data:KERN_SHARE")
pcb_t pcb_list[PARTITION_MAX_NUM];

#pragma DATA_SECTION(part_num, ".data:KERN_SHARE")
static u8 part_num;

void partition_add(partition_conf_t *conf) {
    u32 i, r;
    if (part_num >= PARTITION_MAX_NUM) {
        panic("partition_add: No available partition");
    }
    printf("%s: adding part %d\n", __FUNCTION__, part_num);
    partition_context_t *context = &(pcb_list[part_num].partition_context);
    partition_context_init(context);

    pcb_list[part_num].index = part_num;
    pcb_list[part_num].task_num = conf->task_num;
    pcb_list[part_num].slice_ticks = conf->slice_ticks;
    pcb_list[part_num].slice_ticks_left = 0;
    pcb_list[part_num].target_core = conf->target_core;

    partition_context_load(context);

    for (i = 0; i < conf->task_num; i++) {
        r = OSTaskCreate(
                conf->task_conf_list[i].entry,
                conf->task_conf_list[i].arg,
                conf->task_conf_list[i].stack_ptr,
                conf->task_conf_list[i].priority);
        if (r != 0) {
            printf("OSTaskCreate return %d\n", r);
            panic("Create task failed");
        }
        OSTCBPrioTbl[conf->task_conf_list[i].priority]->OSTCBId = (INT16U) i;
    }

    partition_context_save(context);
    pcb_list[part_num].xmc_id = xmc_segment_allocate(&conf->memory_conf);
    part_num++;
}

void partition_init() {
    part_num = 0;
    partition_current = NULL;
    OS_MemClr((INT8U *) pcb_list, sizeof(pcb_list));
}

void partition_context_init(partition_context_t *context) {
    u8 i;
    OS_TCB *ptcb1;
    OS_TCB *ptcb2;

    /* Misc */
    context->OSTime = 0;
    context->OSIntNesting = 0;
    context->OSLockNesting = 0;
    context->OSTaskCtr = 0;
    context->OSRunning = OS_FALSE;
    context->OSCtxSwCtr = 0;
    context->OSIdleCtr = 0;

    context->OSRdyGrp = 0;
    for (i = 0; i < OS_RDY_TBL_SIZE; i ++) {
        context->OSRdyTbl[i] = 0;
    }

    context->OSPrioCur = 0;
    context->OSPrioHighRdy = 0;
    context->OSTCBHighRdy = NULL;
    context->OSTCBCur = NULL;

    OS_MemClr((INT8U *) &context->OSTCBTbl[0], sizeof(context->OSTCBTbl));
    OS_MemClr((INT8U *) &context->OSTCBPrioTbl[0], sizeof(context->OSTCBPrioTbl));

    for (i = 0u; i < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1u); i++) {
        ptcb1 = &context->OSTCBTbl[i];
        ptcb2 = &context->OSTCBTbl[i + 1];
        ptcb1->OSTCBNext = ptcb2;
    }

    ptcb1 = &context->OSTCBTbl[i];
    ptcb1->OSTCBNext = NULL;

    context->OSTCBList = NULL;
    context->OSTCBFreeList = &context->OSTCBTbl[0];

    OS_MemClr((INT8U *) &context->OSTaskIdleStk[0], sizeof(context->OSTaskIdleStk));
}

void partition_context_save(partition_context_t *context) {
    context->OSTime         = OSTime;

    context->OSIntNesting   = OSIntNesting;
    context->OSLockNesting  = OSLockNesting;

    context->OSTaskCtr      = OSTaskCtr;
    context->OSRunning      = OSRunning;
    context->OSCtxSwCtr     = OSCtxSwCtr;
    context->OSIdleCtr      = OSIdleCtr;

    context->OSRdyGrp       = OSRdyGrp;
    OSRdyTbl       = NULL;

    context->OSPrioCur      = OSPrioCur;
    context->OSPrioHighRdy  = OSPrioHighRdy;
    context->OSTCBHighRdy   = OSTCBHighRdy;
    context->OSTCBCur       = OSTCBCur;

    OSTCBTbl       = NULL;
    OSTCBPrioTbl   = NULL;

    context->OSTCBList      = OSTCBList;
    context->OSTCBFreeList  = OSTCBFreeList;

    OSTaskIdleStk  = NULL;
}

void partition_context_load(partition_context_t *context) {
    OSTime         = context->OSTime;

    OSIntNesting   = context->OSIntNesting;
    OSLockNesting  = context->OSLockNesting;

    OSTaskCtr      = context->OSTaskCtr;
    OSRunning      = context->OSRunning;
    OSCtxSwCtr     = context->OSCtxSwCtr;
    OSIdleCtr      = context->OSIdleCtr;

    OSRdyGrp       = context->OSRdyGrp;
    OSRdyTbl       = context->OSRdyTbl;

    OSPrioCur      = context->OSPrioCur;
    OSPrioHighRdy  = context->OSPrioHighRdy;
    OSTCBHighRdy   = context->OSTCBHighRdy;
    OSTCBCur       = context->OSTCBCur;

    OSTCBTbl       = context->OSTCBTbl;
    OSTCBPrioTbl   = context->OSTCBPrioTbl;

    OSTCBList      = context->OSTCBList;
    OSTCBFreeList  = context->OSTCBFreeList;

    OSTaskIdleStk  = context->OSTaskIdleStk;
}

static inline pcb_t *_core_first_partition() {
    int i;
    for (i = 0; i < part_num; i++) {
        if (pcb_list[i].target_core == core_id) {
            return &(pcb_list[i]);
        }
    }
    return NULL;
}

static inline pcb_t *_core_next_partition() {
    int i;
    for (i = partition_current->index + 1; i < part_num; i++) {
        if (pcb_list[i].target_core == core_id) {
            return &(pcb_list[i]);
        }
    }
    return _core_first_partition();
}

void partition_schedule() {
    if (partition_current == NULL) {
        panic("partition_schedule: current_partition == NULL");
    }
    if (partition_current->slice_ticks_left == 0) {
        printf("%s: part %d slice reduced to zero\n", __FUNCTION__, partition_current->index);
        /* Need to switch to next partition */
        pcb_t *partition_next = _core_next_partition();
        if (partition_next == partition_current) {
            printf("%s: next part is current one\n", __FUNCTION__);
            /* Refill slice */
            partition_current->slice_ticks_left = partition_current->slice_ticks - 1;
            /* Next partition is current, continue */
            return;
        }
        printf("%s: next part is %d\n", __FUNCTION__, partition_next->index);
        OSTCBCur->context_frame = task_context_saved;
        partition_context_save(&(partition_current->partition_context));
        partition_current = partition_next;
        partition_current->slice_ticks_left = partition_current->slice_ticks - 1;
        partition_switch();
        return;
    } else {
        printf("%s: part %d slice from %d -> %d\n", __FUNCTION__, partition_current->index,
                partition_current->slice_ticks_left, partition_current->slice_ticks_left - 1);
        partition_current->slice_ticks_left--;
        return;
    }
}

void partition_switch() {
    printf("partition_switch: switching to part %d\n", partition_current->index);
    partition_context_load(&(partition_current->partition_context));
    xmc_segment_activate(partition_current->xmc_id);
    //xmc_mem_map_dump();
    if (partition_current->partition_context.OSRunning) {
        task_context_saved = OSTCBCur->context_frame;
        if (OSTCBCur != OSTCBHighRdy || OSPrioCur != OSPrioHighRdy) {
            panic("partition_switch: corrupted OSTCBCur");
        }
        OSIntCtxSw();
    } else {
        INT8U y;
        y = OSUnMapTbl[OSRdyGrp];
        OSPrioHighRdy = (INT8U) ((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
        OSPrioCur = OSPrioHighRdy;
        OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
        OSTCBCur = OSTCBHighRdy;
        OSRunning = OS_TRUE;
        task_context_saved = OSTCBCur->context_frame;
        printf("%s: OS_SchedNew yield high ready t%d (pri%d)\n", __FUNCTION__, OSTCBHighRdy->OSTCBId, OSPrioHighRdy);
        OSIntCtxSw();
    }
}


void partition_start() {
    partition_current = _core_first_partition();
    if (partition_current == NULL) {
        panic("CORE HAS NO PARTITION ALLOCATED");
    }
    printf("%s: starting part %d\n", __FUNCTION__, partition_current->index);
    partition_current->slice_ticks_left = partition_current->slice_ticks - 1;
    partition_context_load(&(partition_current->partition_context));
    printf("%s: ucos_context_load done\n", __FUNCTION__);
    xmc_segment_activate(partition_current->xmc_id);
    //xmc_mem_map_dump();
    if (core_id == 1) {
        timer_init();
    }
    printf("%s: timer_init done\n", __FUNCTION__);
    printf("%s: ready to OSStart\n", __FUNCTION__);

    OSStart();
}

