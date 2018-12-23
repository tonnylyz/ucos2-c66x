#ifndef _OS_CPU_H_
#define _OS_CPU_H_

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT extern
#endif

#include <printf.h>

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

typedef unsigned int   OS_STK;

#define  OS_ENTER_CRITICAL() __asm ("\tNOP 5")
#define  OS_EXIT_CRITICAL()  __asm ("\tNOP 5")

#define  OS_STK_GROWTH 1                              /* Stack grows from HIGH to LOW memory */

#define  OS_TASK_SW()  OSCtxSw()

#define panic(_) do { printf(_); while (1) { __asm ("\tNOP"); }  } while(0)

/* Prototypes for functions in os_cpu_s.asm */
extern void OSIntCtxSw();
extern void OSStartHighRdy();
extern void OSCtxSw();

typedef struct
{
    // Task Entry
    INT32U Entry;

    // GPR
    INT32U A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15;
    INT32U B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15;

    // Control Reg
    INT32U AMR;       // Addressing Mode Reg
    INT32U CSR;       // Control Status Reg
    INT32U IER;       // Interrupt Enable Reg
    INT32U ELR;       // Interrupt / Exception Return Ptr
    INT32U TSR;       // Task State Pointer
} context_frame_t;

extern context_frame_t saved_context;

#endif
