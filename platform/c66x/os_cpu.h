/*
 *********************************************************************************************************
 *                                               uC/OS-II
 *                                        The Real-Time Kernel
 *
 *                         (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
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

#ifndef _OS_CPU_H_
#define _OS_CPU_H_

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT extern
#endif

/*
 *********************************************************************************************************
 *                                              DATA TYPES
 *                                         (Compiler Specific)
 *********************************************************************************************************
 */

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed char    INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed short   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed int     INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide in C6711DSP        */

extern cregister volatile unsigned int CSR;
void Disable_int(void);                    //function for disable interrupt
void Enable_int(void);                     //function for enable interrupt

#define  OS_ENTER_CRITICAL() Disable_int() //CSR &= 0xfffe;asm("	nop 4");	/* Disable interrupts*/
#define  OS_EXIT_CRITICAL()  Enable_int()  //CSR |= 0x0001;asm("	nop 4");	/* Enable  interrupts*/

#define  OS_STK_GROWTH 1                              /* Stack grows from HIGH to LOW memory */

#define  OS_TASK_SW()  OSCtxSw()

extern void OSIntCtxSw();
extern void OSStartHighRdy();
extern void OSCtxSw();

unsigned int DSP_C6x_GetCurrentSP(void);
unsigned int DSP_C6x_GetCurrentDP(void);

/*------------------------------------------------------------------------
 * /	THE SP MUST BE ALIGNED ON AN 8-BYTE BOUNDARY.  (Ref. Datasheet of C6711 DSP )
 * /	WHICH means that the framesize has to be a multiple of 8-BYTE.
 * /------------------------------------------------------------------------*/

typedef struct
{
//The start address of the Task (point of the task function)	-------- 4-BYTE
    INT32U start_address;

//All the General-Purpose Registers of the DSP (32 Registers)	-------- 32 X 4-BYTE
    INT32U A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15;
    INT32U B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15;

//Save all Control Registers of the DSP	-------- 4 X 4-BYTE
    INT32U
            AMR,       //Adressing_Mode_Register
            CSR,       //Control_Status_Register
            IER,       //Interrupt_Enable_Register
            IRP;       //Interrupt_Return_Pointer

//A 4-BYTE dummy, just keep the SP is aligned on an 8-BYTE boundary
    INT32U pad;
} INIT_STACK_FRAME;

#endif
