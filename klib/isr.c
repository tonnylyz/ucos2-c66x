#include "isr.h"

#include <partition.h>
#include <timer.h>
#include <intc.h>
#include <uart.h>
#include <xmc.h>
#include <os_cpu.h>
#include <printf.h>
#include <ucos_ii.h>


void OSTaskTimerISR() {
    timer_irq_clear(GP_TASK_TIMER_BASE);
    intc_event_clear(INTC_EVENT_TASK_TIMER);

    OSIntEnter();
    OSTimeTick();
    OSIntExit();
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
        if (no == 0) {
            uart_putc((char) arg[0]);
        } else if (no == 1) {
            uart_puts((char *)arg[0]);
        } else if (no == 2) {
            OSTimeDly(arg[0]);
        } else {
            printf("Unknown System Call %d\n", no);
            panic("\n");
        }
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

    partition_schedule();
}

void OSXMCExceptionISR() {
    printf("current part: %d\n", partition_current->index);
    printf("current task: %d (pri %d)\n", OSTCBCur->OSTCBId, OSTCBCur->OSTCBPrio);
    printf("xmc fault address: [%08x]\n", *((u32 volatile *)XMC_XMPFAR));
    printf("xmc fault status:  [%08x]\n", *((u32 volatile *)XMC_XMPFSR));
    panic("OSXMCExceptionISR called");
}
