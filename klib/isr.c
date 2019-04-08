#include "ipc.h"
#include "apex.h"

#include <partition.h>
#include <timer.h>
#include <intc.h>
#include <uart.h>
#include <xmc.h>
#include <os_cpu.h>
#include <printf.h>
#include <ucos_ii.h>
#include <mailbox.h>


void OSTaskTimerISR(void) {
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
            printf("%08x", arg[0]);
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
        case 20: {
            apex_set_partition_mode((operating_mode_t) arg[0], (return_code_t *) arg[1]);
        } break;
        case 21: {
            apex_get_partition_mode((operating_mode_t *) arg[0], (return_code_t *) arg[1]);
        } break;
        case 22: {
            apex_get_process_id((char *) arg[0], (process_id_t *) arg[1], (return_code_t *) arg[2]);
        } break;
        case 23: {
            apex_get_process_status((process_id_t) arg[0], (process_status_t *) arg[1], (return_code_t *) arg[2]);
        } break;
        case 24: {
            apex_create_process((process_attribute_t *) arg[0], (process_id_t *) arg[1], (return_code_t *) arg[2]);
        } break;
        case 25: {
            apex_set_priority((process_id_t) arg[0], (u8) arg[1], (return_code_t *) arg[2]);
        } break;
        case 26: {
            apex_suspend_self(arg[0], (return_code_t *) arg[1]);
        } break;
        case 27: {
            apex_stop_self();
        } break;
        case 28: {
            apex_get_my_id((process_id_t *) arg[0], (return_code_t *) arg[1]);
        } break;
        case 29: {
            apex_create_sampling_port((sampling_port_name_t) arg[0], arg[1], (port_direction_t) arg[2], arg[3],
                                      (sampling_port_id_t *) arg[4], (return_code_t *) arg[5]);
        } break;
        case 30: {
            apex_write_sampling_port(arg[0], arg[1], arg[2], (return_code_t *) arg[3]);
        } break;
        case 31: {
            apex_read_sampling_port(arg[0], arg[1], (message_size_t *) arg[2], (validity_t *) arg[3],
                                    (return_code_t *) arg[4]);
        } break;
        case 32: {
            apex_get_sampling_port_id((sampling_port_name_t) arg[0], (sampling_port_id_t *) arg[1],
                                      (return_code_t *) arg[2]);
        } break;
        case 33: {
            apex_get_sampling_port_status(arg[0], (sampling_port_status_t *) arg[1], (return_code_t *) arg[2]);
        } break;
        default: {
            printf("Unknown System Call %d\n", no);
            panic("\n");
        }
    }
    return 0;
}

void OSExceptionISR(u32 efr, u32 ierr) {
    if (efr & (1u << 1u)) { /* IXF Internal Exception */
        printf("IXF: ierr [%02x]\n", ierr);
        printf("IXF: elr  [%08x]\n", task_context_saved.ELR);
        panic("IXF occurred.\n");
    } else if (efr & (1u << 30u)) { /* EXF Exception */
        panic("EXF occurred.\n");
    } else if (efr & (1u << 31u)) { /* NXF NMI Exception */
        panic("NXF occurred.\n");
    } else if (efr & (1u << 0u)) { /* SXF aka System Call */
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
    if (task_context_saved.TSR & (1u << 16u)) {
        printf("ELR [%08x]\n", task_context_saved.ELR);
        printf("TSR [%08x]\n", task_context_saved.TSR);
        printf("SP [%08x]\n", task_context_saved.B15);
        printf("DP [%08x]\n", task_context_saved.B14);
        panic("Hardware Exception (HWE) Occurred. Unable to resume!\n");
    }
    panic("Unrecognized Exception\n");
}


void OSPartitionTimerISR(void) {
    timer_irq_clear(GP_PART_TIMER_BASE);
    intc_event_clear(INTC_EVENT_PART_TIMER);

    partition_tick();
}

void OSXMCExceptionISR(void) {
    printf("current part: %d\n", partition_current->identifier);
    printf("current task: %d (pri %d)\n", OSTCBCur->OSTCBId, OSTCBCur->OSTCBPrio);
    printf("xmc fault address: [%08x]\n", *((u32 volatile *)XMC_XMPFAR));
    printf("xmc fault status:  [%08x]\n", *((u32 volatile *)XMC_XMPFSR));
    panic("OSXMCExceptionISR called");
}

void OSMailboxISR(void) {
    mailbox_irq_clear();
    intc_event_clear(INTC_EVENT_MAILBOX);
    u8 pid = (u8) mailbox_receive();
    if (pid >= PARTITION_MAX_NUM) {
        return;
    }
    if (pcb_list[pid].conf->target_core != core_id) {
        return;
    }
    if (pid != partition_current->identifier) {
        return;
    }
    printf("OSMailboxISR -> ipc_scan_change\n");
    ipc_scan_change();
}
