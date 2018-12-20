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
OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
    INIT_STACK_FRAME *stack_frame, *stack_bottom;
    int              *stack_free_ptr;

    /*	THE SP MUST BE ALIGNED ON AN 8-BYTE BOUNDARY.*/
    stack_frame = (INIT_STACK_FRAME *)((int)ptos & ~7);
    stack_bottom = stack_frame;
    stack_frame--;   /* to the bottom of the frame, to init the struct. */

    stack_frame->start_address = (INT32U)task;

    stack_frame->A0 = 0x0A00;
    stack_frame->A1 = 0x0A01;
    stack_frame->A2 = 0x0A02;
    stack_frame->A3 = 0x0A03;
    stack_frame->A4 = (INT32U) pdata;          /* the first argument of C function here*/
    stack_frame->A5 = 0x0A05;
    stack_frame->A6 = 0x0A06;
    stack_frame->A7 = 0x0A07;
    stack_frame->A8 = 0x0A08;
    stack_frame->A9 = 0x0A09;
    stack_frame->A10 = 0x0A10;
    stack_frame->A11 = 0x0A11;
    stack_frame->A12 = 0x0A12;
    stack_frame->A13 = 0x0A13;
    stack_frame->A14 = 0x0A14;
    stack_frame->A15 = 0x0A15;
    stack_frame->B0 = 0x0B00;
    stack_frame->B1 = 0x0B01;
    stack_frame->B2 = 0x0B02;
    stack_frame->B3 = (INT32U) task;           //for cosmetic reason
    stack_frame->B4 = 0x0B04;
    stack_frame->B5 = 0x0B05;
    stack_frame->B6 = 0x0B06;
    stack_frame->B7 = 0x0B07;
    stack_frame->B8 = 0x0B08;
    stack_frame->B9 = 0x0B09;
    stack_frame->B10 = 0x0B10;
    stack_frame->B11 = 0x0B11;
    stack_frame->B12 = 0x0B12;
    stack_frame->B13 = 0x0B13;
    stack_frame->B14 = DSP_C6x_GetCurrentDP(); /* Save current data pointer */
    stack_frame->B15 = (INT32U) stack_bottom;       /* Save the SP */

    stack_frame->AMR = 0;                      /*default value as it was after reset*/
    stack_frame->CSR = 0x0103;                 /* Little Endian(bit8 = 1); PGIE set 1; GIE set 1 */
    stack_frame->IER = 0x4012;                   /* Just set the NMIF bit */
    stack_frame->IRP = 0xabcdabcd;             /* never return, so any value could used here. */

    stack_free_ptr = (int *)stack_frame;
    stack_free_ptr--;     /* Jusr move 4-BYTE(int) to the next free position */

    return (OS_STK *)stack_free_ptr;
}


/*$PAGE*/ /*
 *********************************************************************************************************
 *                                           HANDLE TIMER TICK ISR
 *										   - void OSTickISR(void) -
 *
 * Description: This function is the C6x-DSP Timer0 ISR. And the timer tick should be 10~100 /sec.
 *				It also offers the time tick to uC/OS-II.
 **********************************************************************************************************
 */

extern void ContextSave();
extern void ContextRestore();

extern void irq_clear();
extern void timer_clear_irq();
void OSTickISR(void)
{
    ContextSave();

    printf("OSTickISR called\n");

    timer_clear_irq();
    irq_clear();

    OSIntEnter();
    OSTimeTick();
    OSIntExit();

    ContextRestore();

    asm ("\tNOP	5");
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
