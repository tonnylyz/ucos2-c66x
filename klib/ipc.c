//
// Created by Tonny on 3/23/2019.
//

#include <ucos_ii.h>
#include <printf.h>
#include <partition.h>
#include "ipc.h"

static bool _check_addr(u8 pid, u32 addr, u32 len) {
    u32 high, low;
    memory_conf_t *conf;
    conf = &(pcb_list[pid].conf->memory_conf);
    high = conf->address + conf->size;
    low = conf->address;
    if (addr < low || addr >= high) {
        return false;
    }
    if (addr + len >= high) {
        return false;
    }
    return true;
}

void ipc_receive() {
    OSTCBCur->ipc.is_receiving = true;
    OSTCBCur->ipc.foreign_accessed = false;
    OSTCBCur->ipc.expect_pid = partition_current->identifier;
    OSTaskSuspend(OSPrioCur);
}

void ipc_receive_foreign(u8 pid, u32 addr, u32 max_len) {
    OSTCBCur->ipc.is_receiving = true;
    OSTCBCur->ipc.foreign_accessed = false;
    OSTCBCur->ipc.expect_pid = pid;
    OSTCBCur->ipc.addr = addr;
    OSTCBCur->ipc.max_len = max_len;
    OSTaskSuspend(OSPrioCur);
}

int ipc_send(u8 prio, u32 value) {
    OS_TCB* tcb;
    tcb = partition_current->partition_context.OSTCBPrioTbl[prio];
    if (tcb == NULL) {
        /* prio not exists */
        printf("ipc: prio not exists\n");
        return -1;
    }
    printf("tcb prio %d\n", tcb->OSTCBPrio);
    if (tcb->ipc.is_receiving == false) {
        /* Task not receiving */
        printf("ipc: Task not receiving\n");
        return -2;
    }
    if (tcb->ipc.expect_pid != partition_current->identifier) {
        /* Expect pid mismatch */
        printf("ipc: Expect pid mismatch\n");
        return -3;
    }
    tcb->ipc.value = value;
    tcb->ipc.is_receiving = false;
    tcb->context_frame.A4 = value;
    OSTaskResume(tcb->OSTCBPrio);
    return 0;
}

int ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len) {
    OS_TCB* tcb;
    if (len > IPC_INTER_PARTITION_MAX_LENGTH) {
        return -1;
    }
    if (pid >= PARTITION_MAX_NUM) {
        return -2;
    }
    if (prio > OS_LOWEST_PRIO) {
        return -3;
    }
    if (addr != 0 && _check_addr(partition_current->identifier, addr, len) == false) {
        return -4;
    }
    if (pcb_list[pid].target_core == core_id) {
        tcb = pcb_list[pid].partition_context.OSTCBPrioTbl[prio];
        if (tcb == NULL) {
            return -5;
        }
        if (tcb->ipc.is_receiving == false) {
            return -6;
        }
        if (tcb->ipc.max_len < len) {
            return -7;
        }
        if (tcb->ipc.expect_pid != partition_current->identifier) {
            return -8;
        }
        if (addr != 0) {
            OS_MemCopy(tcb->ipc.buf, (INT8U *) addr, (INT16U) len);
        }
        tcb->ipc.max_len = len;
        tcb->ipc.value = value;
        tcb->ipc.is_receiving = false;
        tcb->context_frame.A4 = value;
        tcb->ipc.foreign_accessed = true; /* task resume when this partition schedule */
        return 0;
    } else {
        printf("need to notify another core.");
    }
    return 0;
}

void ipc_scan_change() {
    u8 i;
    for (i = 0; i <= OS_LOWEST_PRIO; i++) {
        OS_TCB *tcb = partition_current->partition_context.OSTCBPrioTbl[i];
        if (tcb == NULL) {
            continue;
        }
        if (tcb->ipc.foreign_accessed == true) {
            tcb->ipc.foreign_accessed = false;
            if (_check_addr(partition_current->identifier, OSTCBPrioTbl[i]->ipc.addr, OSTCBPrioTbl[i]->ipc.max_len)) {
                OS_MemCopy((INT8U *) OSTCBPrioTbl[i]->ipc.addr, OSTCBPrioTbl[i]->ipc.buf, (INT16U) OSTCBPrioTbl[i]->ipc.max_len);
            }
            if ((tcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY) { /* Task must be suspended                */
                tcb->OSTCBStat &= (INT8U)~(INT8U)OS_STAT_SUSPEND;    /* Remove suspension                     */
                if (tcb->OSTCBStat == OS_STAT_RDY) {                 /* See if task is now ready              */
                    if (tcb->OSTCBDly == 0u) {
                        OSRdyGrp              |= tcb->OSTCBBitY;    /* Yes, Make task ready to run           */
                        OSRdyTbl[tcb->OSTCBY] |= tcb->OSTCBBitX;
                    }
                }
            }
        }
    }
}
