#include "partition.h"
#include <ucos_ii.h>
#include <xmc.h>
#include <timer.h>
#include <intc.h>

pcb_t *current_partition;

pcb_t pcb_list[PARTITION_MAX_NUM];

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
    current_partition = NULL;
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


static pcb_t *_partition_next() {
    u32 idx = current_partition->index;
    return pcb_list + ((idx + 1) % part_num);
}

void partition_schedule() {
    if (current_partition == NULL) {
        panic("partition_schedule: current_partition == NULL");
    }
    if (current_partition->slice_ticks_left == 0) {
        printf("%s: part %d slice reduced to zero\n", __FUNCTION__, current_partition->index);
        /* Need to switch to next partition */
        pcb_t *partition_next = _partition_next();
        if (partition_next == current_partition) {
            printf("%s: next part is current one\n");
            /* Refill slice */
            current_partition->slice_ticks_left = current_partition->slice_ticks - 1;
            /* Next partition is current, continue */
            return;
        }
        printf("%s: next part is %d\n", __FUNCTION__, partition_next->index);
        OSTCBCur->context_frame = saved_context;
        partition_context_save(&(current_partition->partition_context));
        current_partition = partition_next;
        current_partition->slice_ticks_left = current_partition->slice_ticks - 1;
        partition_switch();
        return;
    } else {
        printf("%s: part %d slice from %d -> %d\n", __FUNCTION__, current_partition->index,
                current_partition->slice_ticks_left, current_partition->slice_ticks_left - 1);
        current_partition->slice_ticks_left--;
        return;
    }
}

void partition_switch() {
    printf("partition_switch: switching to part %d\n", current_partition->index);
    partition_context_load(&(current_partition->partition_context));
    xmc_segment_activate(current_partition->xmc_id);
    //xmc_mem_map_dump();
    if (current_partition->partition_context.OSRunning) {
        saved_context = OSTCBCur->context_frame;
        if (OSTCBCur != OSTCBHighRdy || OSPrioCur != OSPrioHighRdy) {
            panic("partition_switch: corrupted OSTCBCur");
        }
        OSIntCtxSw();
    } else {
        /*timer_irq_clear(GP_TASK_TIMER_BASE);
        timer_irq_clear(GP_PART_TIMER_BASE);
        intc_event_clear(INTC_EVENT_TASK_TIMER);
        intc_event_clear(INTC_EVENT_PART_TIMER);
        timer_init();
        printf("%s: timer_init done\n", __FUNCTION__);*/
        INT8U y;
        y = OSUnMapTbl[OSRdyGrp];
        OSPrioHighRdy = (INT8U) ((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
        OSPrioCur = OSPrioHighRdy;
        OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
        OSTCBCur = OSTCBHighRdy;
        OSRunning = OS_TRUE;
        saved_context = OSTCBCur->context_frame;
        printf("%s: OS_SchedNew yield high ready t%d (pri%d)\n", __FUNCTION__, OSTCBHighRdy->OSTCBId, OSPrioHighRdy);
        OSIntCtxSw();
    }
}

void partition_start() {
    current_partition = &(pcb_list[0]);
    printf("%s: starting part %d\n", __FUNCTION__, current_partition->index);
    current_partition->slice_ticks_left = current_partition->slice_ticks - 1;
    partition_context_load(&(current_partition->partition_context));
    printf("%s: ucos_context_load done\n", __FUNCTION__);
    xmc_segment_activate(current_partition->xmc_id);
    //xmc_mem_map_dump();

    timer_init();
    printf("%s: timer_init done\n", __FUNCTION__);
    printf("%s: ready to OSStart\n", __FUNCTION__);

    OSStart();
}

