#include "partition.h"
#include "apex.h"

#include <printf.h>
#include <ucos_ii.h>
#include <xmc.h>
#include <timer.h>
#include <intc.h>
#include <ipc.h>

pcb_t *partition_current;

#pragma DATA_SECTION(pcb_list, ".data:KERN_SHARE")
pcb_t pcb_list[PARTITION_MAX_NUM];

static void _strncpy(char *dst, const char *src, u32 len) {
    u32 i;
    for (i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void partition_register(partition_conf_t *conf) {
    u32 i;
    pcb_t *pcb;
    if (conf->identifier >= PARTITION_MAX_NUM) {
        panic("partition_add: unable to register index from identifier");
    }
    if (conf->task_num > PARTITION_MAX_PROCESS_NUM) {
        panic("partition_add: task number exceeds limit");
    }
    pcb = &pcb_list[conf->identifier];
    pcb->conf = conf;
    pcb->identifier = conf->identifier;
    pcb->target_core = conf->target_core;
    pcb->task_num = conf->task_num;
    pcb->slice_ticks = conf->slice_ticks;
    pcb->slice_ticks_left = 0;
    partition_context_init(&(pcb->context));
    partition_context_load_from(&(pcb->context));
    u16 pid;
    for (i = 0; i < conf->task_num; i++) {
        pid = i + 1;
        OSTaskCreate(
                conf->task_conf_list[i].entry,
                conf->task_conf_list[i].arg,
                conf->task_conf_list[i].stack_ptr,
                conf->task_conf_list[i].priority);
        OSTCBPrioTbl[conf->task_conf_list[i].priority]->OSTCBId = pid;
        pcb->process_list[i].pid = pid;
        pcb->process_list[i].tcb = OSTCBPrioTbl[conf->task_conf_list[i].priority];
        pcb->process_list[i].attributes = (process_attribute_t) {
            /*.name = conf->task_conf_list[i].name,*/
            .deadline = ddl_soft,
            .base_priority = conf->task_conf_list[i].priority,
            .entry_point = (u32) conf->task_conf_list[i].entry,
            .period = 0,
            .stack_size = conf->task_conf_list[i].stack_size,
            .time_capacity = 0,
        };
        _strncpy(pcb->process_list[i].attributes.name, conf->task_conf_list[i].name, APEX_NAME_MAX_LEN);
        pcb->process_list[i].current_priority = conf->task_conf_list[i].priority;
        pcb->process_list[i].deadline_time = 0;
        pcb->process_list[i].process_state = ps_ready;

    }
    partition_context_save_into(&(pcb->context));
    pcb->xmc_id = xmc_segment_allocate(&conf->memory_conf);
}

void partition_init() {
    int i;
    partition_current = NULL;
    OS_MemClr((INT8U *) pcb_list, sizeof(pcb_list));
    for (i = 0; i < PARTITION_MAX_NUM; i++) {
        pcb_list[i].target_core = 255u;
    }
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

void partition_context_save_into(partition_context_t *context) {
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

void partition_context_load_from(partition_context_t *context) {
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

static pcb_t *_core_first_partition() {
    int i;
    for (i = 0; i < PARTITION_MAX_NUM; i++) {
        if (pcb_list[i].target_core == core_id) {
            return &(pcb_list[i]);
        }
    }
    return NULL;
}

static pcb_t *_core_next_partition() {
    int i;
    for (i = partition_current->identifier + 1; i < PARTITION_MAX_NUM; i++) {
        if (pcb_list[i].target_core == core_id) {
            return &(pcb_list[i]);
        }
    }
    return _core_first_partition();
}

void partition_start() {
    pcb_t *part;
    part = _core_first_partition();
    if (part == NULL) {
        panic("No partition runs at this core.\n");
    }
    part->slice_ticks_left = part->slice_ticks - 1;
    if (core_id == 1) {
        timer_init();
    }
    partition_current = part;
    xmc_segment_activate(part->xmc_id);
    partition_context_load_from(&(part->context));
    OSStart();
}

void partition_tick() {
    if (partition_current->slice_ticks_left == 0) {
        pcb_t *partition_next = _core_next_partition();
        partition_switch(partition_current, partition_next);
        return;
    } else {
        partition_current->slice_ticks_left--;
        return;
    }
}

void partition_run(pcb_t *pcb) {
    partition_current = pcb;
    xmc_segment_activate(pcb->xmc_id);
    partition_context_load_from(&(pcb->context));
    if (OSRunning == OS_TRUE) {
        ipc_scan_change();
        task_context_saved = OSTCBCur->context_frame;
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
        OSIntCtxSw();
    }
}

void partition_switch(pcb_t *prev, pcb_t *next) {
    printf("partition_switch from %d to %d\n", prev->identifier, next->identifier);
    if (prev == next) {
        prev->slice_ticks_left = prev->slice_ticks - 1;
        return;
    }
    OSTCBCur->context_frame = task_context_saved;
    partition_context_save_into(&(prev->context));
    next->slice_ticks_left = next->slice_ticks - 1;
    partition_run(next);
}
