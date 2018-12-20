/*
 *********************************************************************************************************
 *                                               uC/OS-II
 *                                         The Real-Time Kernel
 *
 *                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
 *                                          All Rights Reserved
 *
 *                                     TI C6711 DSP Specific code
 *                                          Little Endian Mode
 *
 *                                      Code Composer Studio V2.2
 *
 * File         : OS_CPU.H
 * By	       : Ming Zeng (ming@zming.net) 2003.11
 * ; History      : Kenneth Blake   (the author of V2.00 port for C6211)
 *********************************************************************************************************
 */
#define  OS_CPU_GLOBALS

#include <ucos_ii.h>
#include <os_cpu.h>
#include <snprintf.h>


/*
 *********************************************************************************************************
 *                                        INITIALIZE A TASK'S STACK
 *
 * Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
 *              stack frame of the task being created.  This function is highly processor specific.
 *
 * Arguments  : task          is a pointer to the task code
 *
 *              pdata         is a pointer to a user supplied data area that will be passed to the task
 *                            when the task first executes.--
 *
 *              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
 *                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then
 *                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
 *                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
 *                            of the stack.
 *
 *              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
 *                            (see uCOS_II.H for OS_TASK_OPT_???).
 *
 * Returns    : Always returns the location of the new top-of-stack' once the processor registers have
 *              been placed on the stack in the proper order.
 *
 * Note(s)    : Interrupts are enabled when your task starts executing.
 *********************************************************************************************************
 */
OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT32U opt)
{
    context_frame_t *frame;
    frame = (context_frame_t *)opt;
    frame->Entry = (INT32U)task; // Task Entry

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
    frame->B14 = DSP_C6x_GetCurrentDP(); // Data Ptr
    frame->B15 = ((INT32U)ptos & ~7); // Stack Ptr

    frame->AMR = 0; // reset value (linear)
    frame->CSR = 0b100000011;
             /*  PWRD = 0   No power down
              *  EN = 1     Little endian
              *  PGIE = 1   Interrupt will be enabled after return from interrupt
              *  GIE = 1    Enable all interrupts
              * */
    frame->IER = 0b1111111111110011;
             /* Enable INT4 ~ INT15, NMI
              * */

    frame->ELR = (INT32U) task; // Return address (from Int/Exc)

    frame->TSR = 0b1001111;
              /*  CXM = 1       User Mode
               *  XEN = 1       Enable all maskable exceptions
               *  GEE = 1       Enable all exceptions
               *  SGIE = 1      Enable RINT instruction
               * */


    return (OS_STK *) ((INT32U)ptos & ~7);
}


extern void irq_clear();
extern void timer_irq_clear();

extern cregister volatile unsigned int ITSR;
extern cregister volatile unsigned int NTSR;
extern cregister volatile unsigned int EFR;
extern cregister volatile unsigned int ECR;

void OSTimerISR() {
    printf("OSTimerISR called\n");
    printf("ITSR [%08x]\n", ITSR);
    printf("EFR [%08x]\n", EFR);
    timer_irq_clear();
    irq_clear();

    OSIntEnter();
    OSTimeTick();
    OSIntExit();

    // Clear IB bit
    saved_context.TSR &= ~(1u << 15);
    printf("Return from OSTimerISR\n");
}

void OSExceptionISR() {
    printf("OSExceptionISR called\n");
    printf("NTSR [%08x]\n", NTSR);
    printf("EFR [%08x]\n", EFR);

    // Clear SXF (syscall) bit
    ECR = 0b1;

    // Clear IB bit
    saved_context.TSR &= ~(1u << 15);
}

#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSInitHookBegin(void)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSInitHookEnd(void)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0
void OSTaskCreateHook(OS_TCB *ptcb)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0
void OSTaskDelHook(OS_TCB *ptcb)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void OSTaskIdleHook(void)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0
void OSTaskStatHook(void)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0
void OSTaskSwHook(void)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSTCBInitHook(OS_TCB *ptcb)
{
}
#endif

#if OS_CPU_HOOKS_EN > 0
void OSTimeTickHook(void)
{
}
#endif



#if OS_CPU_HOOKS_EN > 0
void OSTaskReturnHook(OS_TCB *ptcb)
{
}
#endif
