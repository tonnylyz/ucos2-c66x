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

typedef unsigned int   OS_STK;

#define  OS_ENTER_CRITICAL() asm(" NOP")
#define  OS_EXIT_CRITICAL()  asm(" NOP")

#define  OS_STK_GROWTH 1                              /* Stack grows from HIGH to LOW memory */

#define  OS_TASK_SW()  OSCtxSw()

/* Prototypes for functions in os_cpu_s.asm */
extern void OSIntCtxSw(void);
extern void OSStartHighRdy(void);
extern void OSCtxSw(void);


#include <types.h>

extern u8 core_id;
extern u8 CPURegisterDNUM(void);

extern void InstructionCounterStart();
extern u32  InstructionCounterTSCL();
extern u32  InstructionCounterTSCH();

typedef struct
{
    INT32U reserved;

    INT32U A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15;
    INT32U B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15;

    INT32U ELR;       // Interrupt / Exception Return Ptr
    INT32U TSR;       // Task State Pointer
} task_context_t;

extern task_context_t task_context_saved;

#endif
