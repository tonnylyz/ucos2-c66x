#include <timer.h>
#include <intc.h>
#include <uart.h>
#include <xmc.h>
#include <os_cpu.h>
#include <printf.h>
#include <ucos_ii.h>

void OSTaskTimerISR(void) {
    timer_irq_clear(GP_TASK_TIMER_BASE);
    intc_event_clear(INTC_EVENT_TASK_TIMER);

    if (OSRunning == OS_FALSE) {
        return;
    }
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
        default: {
            printf("Unknown System Call %d\n", no);
            panic("\n");
        }
    }
    return 0;
}

void OSExceptionISR(u32 efr, u32 ierr) {
    if (efr & (1u << 1u)) { /* IXF Internal Exception */
        printf("IXF: ierr      [%02x]\n", ierr);
        printf("IXF: task prio [%d]\n", OSTCBCur->OSTCBPrio);
        printf("IXF: sp        [%08x]\n", task_context_saved.B15);
        printf("IXF: elr       [%08x]\n", task_context_saved.ELR);
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

void OSXMCExceptionISR(void) {
    printf("[OSXMCExceptionISR][W] task (prio %d) tried to access kernel memory [%08x] (PC->[%08x]), killed\n",
                OSPrioCur, *((u32 volatile *)XMC_XMPFAR), task_context_saved.ELR
            );
    OSTaskDel(OSPrioCur);
}

