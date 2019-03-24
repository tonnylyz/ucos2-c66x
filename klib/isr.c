#include "isr.h"
#include "ipc.h"

#include <partition.h>
#include <timer.h>
#include <intc.h>
#include <uart.h>
#include <xmc.h>
#include <os_cpu.h>
#include <printf.h>
#include <ucos_ii.h>
#include <mailbox.h>


void OSTaskTimerISR() {
    timer_irq_clear(GP_TASK_TIMER_BASE);
    intc_event_clear(INTC_EVENT_TASK_TIMER);

    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

static u32 _handle_syscall(u32 no, u32* arg) {
    switch (no) {
        case 0: {
            uart_putc((char) arg[0]);
        } break;
        case 1: {
            uart_puts((char *)arg[0]);
        } break;
        case 2: {
            OSTimeDly(arg[0]);
        } break;
        case 3: {
            return OSTimeDlyHMSM((INT8U) arg[0], (INT8U) arg[1], (INT8U) arg[2], (INT16U) arg[3]);
        } //break;
        case 4: {
            return OSTimeDlyResume((INT8U) arg[0]);
        } //break;
        case 5: {
            return OSTimeGet();
        } //break;
        case 6: {
            OSTimeSet(arg[0]);
        } break;
        case 7: {
            return OSTaskChangePrio((INT8U) arg[0], (INT8U) arg[1]);
        } //break;
        case 8: {
            return OSTaskDel((INT8U) arg[0]);
        } //break;
        case 9: {
            return OSTaskDelReq((INT8U) arg[0]);
        } //break;
        case 10: {
            return OSTaskResume((INT8U) arg[0]);
        } //break;
        case 11: {
            return OSTaskStkChk((INT8U) arg[0], (OS_STK_DATA *) arg[1]);
        } //break;
        case 12: {
            return OSTaskSuspend((INT8U) arg[0]);
        } //break;
        case 13: {
            return OSTaskQuery((INT8U) arg[0], (OS_TCB *) arg[1]);
        } //break;
        case 14: {
            return OSTaskRegGet((INT8U) arg[0], (INT8U) arg[1], (INT8U *) arg[2]);
        } //break;
        case 15: {
            OSTaskRegSet((INT8U) arg[0], (INT8U) arg[1], arg[2], (INT8U *) arg[3]);
        } break;
        case 16: {
            ipc_receive();
        } break;
        case 17: {
            return (u32) ipc_send((u8) arg[0], arg[1]);
        } //break;
        case 18: {
            ipc_receive_foreign((u8) arg[0], arg[1], arg[2]);
        }
        case 19: {
            return (u32) ipc_send_foreign((u8) arg[0], (u8) arg[1], arg[2], arg[3], arg[4]);
        }
        default: {
            printf("Unknown System Call %d\n", no);
            panic("\n");
        }
    }
    return 0;
}

void OSExceptionISR(u32 efr, u32 ierr) {
    if (efr & (1u << 1)) { /* IXF Internal Exception */
        printf("IXF: ierr [%02x]\n", ierr);
        printf("IXF: elr  [%08x]\n", task_context_saved.ELR);
        panic("IXF occurred.\n");
    } else if (efr & (1u << 30)) { /* EXF Exception */
        panic("EXF occurred.\n");
    } else if (efr & (1u << 31)) { /* NXF NMI Exception */
        panic("NXF occurred.\n");
    } else if (efr & (1u << 0)) { /* SXF aka System Call */
        u32 no, arg[8];
        arg[0] = task_context_saved.A4;
        arg[1] = task_context_saved.B4;
        arg[2] = task_context_saved.A6;
        arg[3] = task_context_saved.B6;
        arg[4] = task_context_saved.A8;
        arg[5] = task_context_saved.B8;
        arg[6] = task_context_saved.A10;
        arg[7] = task_context_saved.B10;
        no = task_context_saved.A12;
        task_context_saved.A4 = _handle_syscall(no, arg);
        return;
    }
    if (task_context_saved.TSR & (1u << 16)) {
        printf("ELR [%08x]\n", task_context_saved.ELR);
        printf("TSR [%08x]\n", task_context_saved.TSR);
        printf("SP [%08x]\n", task_context_saved.B15);
        printf("DP [%08x]\n", task_context_saved.B14);
        panic("Hardware Exception (HWE) Occurred. Unable to resume!\n");
    }
    panic("Unrecognized Exception\n");
}


void OSPartitionTimerISR() {
    timer_irq_clear(GP_PART_TIMER_BASE);
    intc_event_clear(INTC_EVENT_PART_TIMER);

    partition_tick();
}

void OSXMCExceptionISR() {
    printf("current part: %d\n", partition_current->identifier);
    printf("current task: %d (pri %d)\n", OSTCBCur->OSTCBId, OSTCBCur->OSTCBPrio);
    printf("xmc fault address: [%08x]\n", *((u32 volatile *)XMC_XMPFAR));
    printf("xmc fault status:  [%08x]\n", *((u32 volatile *)XMC_XMPFSR));
    panic("OSXMCExceptionISR called");
}

void OSMailboxISR() {
    mailbox_irq_clear();
    intc_event_clear(INTC_EVENT_MAILBOX);
    u8 pid = (u8) mailbox_receive();
    if (pid >= PARTITION_MAX_NUM) {
        return;
    }
    if (pcb_list[pid].target_core != core_id) {
        return;
    }
    if (pid != partition_current->identifier) {
        return;
    }
    printf("OSMailboxISR -> ipc_scan_change\n");
    ipc_scan_change();
}
