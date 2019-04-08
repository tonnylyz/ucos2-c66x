#include <ucos_ii.h>
#include <printf.h>
#include <partition.h>
#include <spinlock.h>
#include <mailbox.h>
#include "ipc.h"

static bool _check_addr(u8 pid, u32 addr, u32 len) {
    u32 high, low;
    high = pcb_list[pid].conf->memory_addr + pcb_list[pid].conf->memory_size;
    low = pcb_list[pid].conf->memory_addr;
    if (addr < low || addr >= high) {
        return false;
    }
    if (addr + len >= high) {
        return false;
    }
    return true;
}

static inline void _lock(u8 pid, u8 prio) {
    u8 n;
    n = (pid << 6) + prio;
    spinlock_lock(n);
}

static inline void _unlock(u8 pid, u8 prio) {
    u8 n;
    n = (pid << 6) + prio;
    spinlock_unlock(n);
}

void ipc_receive(void) {
    _lock(partition_current->identifier, OSPrioCur);
    OSTCBCur->ipc.is_receiving = true;
    OSTCBCur->ipc.foreign_accessed = false;
    OSTCBCur->ipc.expect_pid = partition_current->identifier;
    _unlock(partition_current->identifier, OSPrioCur);
    OSTaskSuspend(OSPrioCur);
}

void ipc_receive_foreign(u8 pid, u32 addr, u32 max_len) {
    _lock(partition_current->identifier, OSPrioCur);
    OSTCBCur->ipc.is_receiving = true;
    OSTCBCur->ipc.foreign_accessed = false;
    OSTCBCur->ipc.expect_pid = pid;
    OSTCBCur->ipc.addr = addr;
    OSTCBCur->ipc.max_len = max_len;
    _unlock(partition_current->identifier, OSPrioCur);
    OSTaskSuspend(OSPrioCur);
}

int ipc_send(u8 prio, u32 value) {
    OS_TCB *tcb;
    _lock(partition_current->identifier, prio);
    tcb = partition_current->context.OSTCBPrioTbl[prio];

    if (tcb == NULL) {
        _unlock(partition_current->identifier, prio);
        return IPC_ERROR_DEST_TASK_NOT_EXISTS;
    }
    if (tcb->ipc.is_receiving == false) {
        _unlock(partition_current->identifier, prio);
        return IPC_ERROR_DEST_TASK_NOT_RECEIVING;
    }
    if (tcb->ipc.expect_pid != partition_current->identifier) {
        _unlock(partition_current->identifier, prio);
        return IPC_ERROR_EXPECT_PID_MISMATCH;
    }
    tcb->ipc.value = value;
    tcb->ipc.is_receiving = false;
    tcb->context_frame.A4 = value;

    _unlock(partition_current->identifier, prio);
    task_context_saved.A4 = 0;
    OSTaskResume(tcb->OSTCBPrio);
    return 0;
}

int ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len) {
    OS_TCB *tcb;
    if (addr != 0 && len > IPC_INTER_PARTITION_MAX_LENGTH) {
        return IPC_ERROR_MESSAGE_TOO_LARGE;
    }
    if (pid >= PARTITION_MAX_NUM || pcb_list[pid].identifier != pid) {
        return IPC_ERROR_DEST_PART_NOT_EXISTS;
    }
    if (prio > OS_LOWEST_PRIO) {
        return IPC_ERROR_DEST_TASK_NOT_EXISTS;
    }
    if (addr != 0 && _check_addr(partition_current->identifier, addr, len) == false) {
        return IPC_ERROR_ADDRESS_BOUNDARY;
    }
    _lock(pid, prio);
    tcb = pcb_list[pid].context.OSTCBPrioTbl[prio];
    if (tcb == NULL) {
        _unlock(pid, prio);
        return IPC_ERROR_DEST_TASK_NOT_EXISTS;
    }
    if (tcb->ipc.is_receiving == false) {
        _unlock(pid, prio);
        return IPC_ERROR_DEST_TASK_NOT_RECEIVING;
    }
    if (tcb->ipc.max_len < len) {
        _unlock(pid, prio);
        return IPC_ERROR_MESSAGE_TOO_LARGE;
    }
    if (tcb->ipc.expect_pid != partition_current->identifier) {
        _unlock(pid, prio);
        return IPC_ERROR_EXPECT_PID_MISMATCH;
    }
    if (addr != 0) {
        OS_MemCopy(tcb->ipc.buf, (INT8U *) addr, (INT16U) len);
    }
    tcb->ipc.max_len = len;
    tcb->ipc.value = value;
    tcb->ipc.is_receiving = false;
    tcb->context_frame.A4 = value;
    tcb->ipc.foreign_accessed = true; /* task resume when this partition schedule */

    _unlock(pid, prio);
    if (pcb_list[pid].conf->target_core != core_id) {
        mailbox_send(pid);
    }
    return 0;
}

void ipc_scan_change(void) {
    u8 i;
    for (i = 0; i < partition_current->task_index; i++) {
        OS_TCB *tcb = partition_current->process_list[i].tcb;
        _lock(partition_current->identifier, i);
        if (tcb->ipc.foreign_accessed == true) {
            tcb->ipc.foreign_accessed = false;
            if (_check_addr(partition_current->identifier, tcb->ipc.addr, tcb->ipc.max_len)) {
                OS_MemCopy((INT8U *) tcb->ipc.addr, tcb->ipc.buf, (INT16U) tcb->ipc.max_len);
            }
            if ((tcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY) {
                tcb->OSTCBStat &= (INT8U) ~(INT8U) OS_STAT_SUSPEND;
                if (tcb->OSTCBStat == OS_STAT_RDY) {
                    if (tcb->OSTCBDly == 0u) {
                        OSRdyGrp |= tcb->OSTCBBitY;
                        OSRdyTbl[tcb->OSTCBY] |= tcb->OSTCBBitX;
                    }
                }
            }
        }
        _unlock(partition_current->identifier, i);
    }
}
