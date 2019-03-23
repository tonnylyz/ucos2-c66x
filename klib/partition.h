#ifndef UCOS2_C66X_PARTITION_H
#define UCOS2_C66X_PARTITION_H

#include <def.h>
#include <types.h>
#include <ucos_ii.h>
#include "partition_conf.h"

#define PARTITION_MAX_NUM 8
#define PARTITION_MAX_TASK_NUM (OS_MAX_TASKS + 1)

typedef struct {
    u32 OSCtxSwCtr;
    u8 OSIntNesting;
    u8 OSLockNesting;

    u8 OSPrioCur;
    u8 OSPrioHighRdy;

    OS_PRIO OSRdyGrp;
    OS_PRIO OSRdyTbl[OS_RDY_TBL_SIZE];

    BOOLEAN OSRunning;

    u8 OSTaskCtr;
    volatile u32 OSIdleCtr;

    OS_STK OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];

    OS_TCB *OSTCBCur;
    OS_TCB *OSTCBFreeList;
    OS_TCB *OSTCBHighRdy;
    OS_TCB *OSTCBList;
    OS_TCB *OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];
    OS_TCB OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];

    u32 OSTime;
} partition_context_t;

typedef struct {
    u8 identifier;
    u8 xmc_id;
    u8 task_num;

    partition_context_t partition_context;

    u32 slice_ticks;
    u32 slice_ticks_left;

    u8 target_core;
    bool ipc_need_resume;

    /* APEX COMPLIANCE */
    operating_mode_t operating_mode;
    start_condition_t start_condition;
    u8 lock_level;
    partition_conf_t *conf;

} pcb_t;

extern pcb_t pcb_list[];

extern pcb_t *partition_current;

void partition_init();

void partition_register(partition_conf_t *conf);

void partition_switch(pcb_t *prev, pcb_t *next);

void partition_start();

void partition_run(pcb_t *pcb);

void partition_context_init(partition_context_t *context);

void partition_context_save_into(partition_context_t *context);

void partition_context_load_from(partition_context_t *context);

void partition_tick();

#endif //UCOS2_C66X_PARTITION_H
