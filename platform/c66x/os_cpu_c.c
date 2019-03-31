#define  OS_CPU_GLOBALS

#include <ucos_ii.h>
#include <os_cpu.h>
#include <types.h>

task_context_t task_context_saved __attribute__ ((aligned (1024)));

extern u32 __TI_STATIC_BASE;

OS_STK *OSTaskStkInit(void (*task)(void *), void *pdata, OS_STK *ptos, INT16U opt, task_context_t *frame)
{
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
