#define  OS_CPU_GLOBALS

#include <ucos_ii.h>
#include <os_cpu.h>
#include <types.h>
#include <printf.h>
#include <uart.h>
#include <timer.h>
#include <intc.h>
#include <partition.h>

context_frame_t saved_context __attribute__ ((aligned (1024)));

extern u32 __TI_STATIC_BASE;

OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT32U opt)
{
    context_frame_t *frame;
    frame = (context_frame_t *)opt;

    frame->A0 = 0x0A00;
    frame->A1 = 0x0A01;
    frame->A2 = 0x0A02;
    frame->A3 = 0x0A03;
    frame->A4 = (INT32U) pdata; // Task Parameter
    frame->A5 = 0x0A05;
    frame->A6 = 0x0A06;
    frame->A7 = 0x0A07;
    frame->A8 = 0x0A08;
    frame->A9 = 0x0A09;
    frame->A10 = 0x0A10;
    frame->A11 = 0x0A11;
    frame->A12 = 0x0A12;
    frame->A13 = 0x0A13;
    frame->A14 = 0x0A14;
    frame->A15 = 0x0A15;
    frame->B0 = 0x0B00;
    frame->B1 = 0x0B01;
    frame->B2 = 0x0B02;
    frame->B3 = (INT32U) task; // Return to Entry
    frame->B4 = 0x0B04;
    frame->B5 = 0x0B05;
    frame->B6 = 0x0B06;
    frame->B7 = 0x0B07;
    frame->B8 = 0x0B08;
    frame->B9 = 0x0B09;
    frame->B10 = 0x0B10;
    frame->B11 = 0x0B11;
    frame->B12 = 0x0B12;
    frame->B13 = 0x0B13;
    frame->B14 = __TI_STATIC_BASE;
    frame->B15 = ((INT32U)ptos & ~7); // Stack Ptr

    frame->ELR = (INT32U) task; // Return address (from Int/Exc)

    frame->TSR = 0b1001111;
              /*  CXM = 1       User Mode
               *  XEN = 1       Enable all maskable exceptions
               *  GEE = 1       Enable all exceptions
               *  SGIE = 1      Enable RINT instruction
               * */

    return (OS_STK *) ((INT32U)ptos & ~7);
}

void OSTaskTimerISR() {
    timer_irq_clear(GP_TASK_TIMER_BASE);
    intc_event_clear(INTC_EVENT_TASK_TIMER);

    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

//extern cregister volatile unsigned int NRP;

void OSExceptionISR(u32 efr, u32 ierr) {
    if (efr & (1u << 1)) { /* IXF Internal Exception */
        printf("IXF: ierr [%02x]\n", ierr);
        printf("IXF: elr  [%08x]\n", saved_context.ELR);
        panic("IXF occurred.\n");
    } else if (efr & (1u << 30)) { /* EXF Exception */
        panic("EXF occurred.\n");
    } else if (efr & (1u << 31)) { /* NXF NMI Exception */
        panic("NXF occurred.\n");
    } else if (efr & (1u << 0)) { /* SXF aka System Call */
        u32 no, arg[8];
        arg[0] = saved_context.A4;
        arg[1] = saved_context.B4;
        arg[2] = saved_context.A6;
        arg[3] = saved_context.B6;
        arg[4] = saved_context.A8;
        arg[5] = saved_context.B8;
        arg[6] = saved_context.A10;
        arg[7] = saved_context.B10;
        no = saved_context.A12;
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
    if (saved_context.TSR & (1u << 16)) {
        printf("ELR [%08x]\n", saved_context.ELR);
        printf("TSR [%08x]\n", saved_context.TSR);
        printf("SP [%08x]\n", saved_context.B15);
        printf("DP [%08x]\n", saved_context.B14);
        panic("Hardware Exception (HWE) Occurred. Unable to resume!\n");
    }
    panic("Unrecognized Exception\n");
}


void OSPartitionTimerISR() {
    printf("OSPartitionTimerISR called\n");
    timer_irq_clear(GP_PART_TIMER_BASE);
    intc_event_clear(INTC_EVENT_PART_TIMER);

    partition_schedule();
}

#if OS_CPU_HOOKS_EN
void OSInitHookBegin(void)
{
}

void OSInitHookEnd(void)
{
}

void OSTaskCreateHook(OS_TCB *ptcb)
{
}

void OSTaskDelHook(OS_TCB *ptcb)
{
}

void OSTaskIdleHook(void)
{
    /* THIS IS USELESS */
}

void OSTaskStatHook(void)
{
}

void OSTaskSwHook(void)
{
}

void OSTCBInitHook(OS_TCB *ptcb)
{
}

void OSTimeTickHook(void)
{
}

void OSTaskReturnHook(OS_TCB *ptcb)
{
}
#endif
