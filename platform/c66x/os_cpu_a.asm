;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
;                                          All Rights Reserved
;
;                                     TI C6711 DSP Specific code
;                                          Little Endian Mode
;
;                                      Code Composer Studio V2.2
;
; File         : OS_CPU.H
; By	       : Ming Zeng (ming@zming.net) 2003.11
; History      : Kenneth Blake 	(the author of V2.00 port for C6211)
;********************************************************************************************************

;********************************************************************************************************
;                                    PUBLIC and EXTERNAL REFERENCES
;********************************************************************************************************

FP	.set	A15
DP	.set	B14
SP	.set	B15
    .global	$bss


_StartAddress .set 0+4  ;structrue of task/int stack
_A0  .set  4        +4
_A1  .set  8        +4
_A2  .set  12       +4
_A3  .set  16       +4
_A4  .set  20       +4
_A5  .set  24       +4
_A6  .set  28       +4
_A7  .set  32       +4
_A8  .set  36       +4
_A9  .set  40       +4
_A10 .set  44       +4
_A11 .set  48       +4
_A12 .set  52       +4
_A13 .set  56       +4
_A14 .set  60       +4
_A15 .set  64       +4
_B0  .set  68       +4
_B1  .set  72       +4
_B2  .set  76       +4
_B3  .set  80       +4
_B4  .set  84       +4
_B5  .set  88       +4
_B6  .set  92       +4
_B7  .set  96       +4
_B8  .set  100      +4
_B9  .set  104      +4
_B10 .set  108      +4
_B11 .set  112      +4
_B12 .set  116      +4
_B13 .set  120      +4
_B14 .set  124      +4
_B15 .set  128      +4
_AMR .set  132      +4
_CSR .set  136      +4
_IER .set  140      +4
_IRP .set  144      +4
_DUMMY .set 148     +4
_Framesize .set 148 +4

    .global OSStartHighRdy
    .global OSCtxSw
    .global OSIntCtxSw

    .global Disable_int ;
    .global Enable_int ;

    .global OSTaskSwHook		; extern function & var from other objects
    .global OSTCBHighRdy
    .global OSTCBCur
    .global OSPrioCur
    .global OSPrioHighRdy
    .global OSRunning

    .global  DSP_C6x_GetCurrentDP  ; It's used in OSTaskStkInit function
    .global  DSP_C6x_GetCurrentSP  ; It's used in any ISR function

;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note : OSStartHighRdy() MUST:
;           a) Call OSTaskSwHook() then,
;           b) Set OSRunning to TRUE,
;           c) Switch to the highest priority task.
;*********************************************************************************************************

OSStartHighRdy:

    CALL	OSTaskSwHook
    NOP		3
    MVKL	Back_Point,B3
    MVKH	Back_Point,B3

Back_Point:	;return here from OSTaskSwHook

    ;OSRunning = TRUE
    MVK		1,B1
    STB		   B1,*+DP(OSRunning)

    ; StackPointer = OSTCBHighRdy->OSTCBStkPtr
    LDW		*+DP(OSTCBHighRdy),B4
    nop    4
    LDW        *B4,SP
    nop    4

    ;pop current task's context from stack
    LDW		*+SP(_StartAddress),B3  ;  b3 start address
    LDW		*+SP(_AMR),B10          ;   B10 = AMR
    LDW		*+SP(_CSR),B9           ;   B9  = CSR
    LDW		*+SP(_IER),B8           ;   B8  = IER

    mvk     0,B13
    mvc		B13,IER

    LDW		*+SP(_A0),A0  ;
    LDW		*+SP(_A1),A1  ;
    LDW		*+SP(_A2),A2  ;
    LDW		*+SP(_A3),A3  ;
    LDW		*+SP(_A4),A4  ;
    LDW		*+SP(_A5),A5  ;
    LDW		*+SP(_A6),A6  ;
    LDW		*+SP(_A7),A7  ;
    LDW		*+SP(_A8),A8  ;
    LDW		*+SP(_A9),A9  ;
    LDW		*+SP(_A10),A10 ;
    LDW		*+SP(_A11),A11 ;
    LDW		*+SP(_A12),A12 ;
    LDW		*+SP(_A13),A13 ;
    LDW		*+SP(_A14),A14 ;
    LDW		*+SP(_A15),A15 ;

    LDW		*+SP(_B0),B0  ;
    LDW		*+SP(_B1),B1  ;
    LDW		*+SP(_B2),B2  ;

    LDW		*+SP(_B4),B4  ;
    LDW		*+SP(_B5),B5  ;
    LDW		*+SP(_B6),B6  ;
    LDW		*+SP(_B7),B7  ;

    LDW		*+SP(_B11),B11  ;
    LDW		*+SP(_B12),B12  ;
    LDW		*+SP(_B13),B13 ;
    LDW		*+SP(_B14),B14  ;


    or		3,b9,B9				; Set GIE
    mvc		B9,CSR				; set csr if IRP did not put a GIE
    mvc		B10,AMR				; Restore AMR
    LDW		*+SP(_B9),B9  ;
    LDW		*+SP(_B10),B10  ;

    LDW		*+SP(_B3),B3  ;
    LDW		*+SP(_B8),B8  ;
    LDW		*+SP(_B15),B15  ;
    B			b3             ;
    mvc			B8,IER         ;
    nop      	5


;*********************************************************************************************************
;
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
;*********************************************************************************************************

;	.align	32

OSCtxSw:
    addk		(-_Framesize),SP   ; Push one Frame

    STW        B3,*+SP(_StartAddress)  ;  b3 start address
    STW        B0,*+SP(_B0)  ;
||  mvc      AMR,B0
    STW        B0,*+SP(_AMR)  ;
||  mvc      CSR,B0
    STW        B0,*+SP(_CSR)  ;
||  mvc      IER,B0
    STW        B0,*+SP(_IER)  ;
||  mvc      IRP,B0
    STW        B0,*+SP(_IRP)  ;

    STW        A0,*+SP(_A0)  ;
||  MV      SP,A0                   ; A0 = SP
    STW        A1,*+SP(_A1)  ;
||  addk    (_Framesize),A0     ; Correct SP for POP of TCB
    STW        A0,*+SP(_B15) ;  Save correct SP


    STW        A2,*+SP(_A2)  ;
    STW        A3,*+SP(_A3)  ;
    STW        A4,*+SP(_A4)  ;
    STW        A5,*+SP(_A5)  ;
    STW        A6,*+SP(_A6)  ;
    STW        A7,*+SP(_A7)  ;
    STW        A8,*+SP(_A8)  ;
    STW        A9,*+SP(_A9)  ;
    STW        A10,*+SP(_A10) ;
    STW        A11,*+SP(_A11) ;
    STW        A12,*+SP(_A12)  ;
    STW        A13,*+SP(_A13)  ;
    STW        A14,*+SP(_A14)  ;
    STW        A15,*+SP(_A15)  ;

    LDW        *+DP(OSTCBCur),B0     ; B0 = OSTCBCur
    STW        B1,*+SP(_B1)  ;
    STW        B2,*+SP(_B2)  ;
    STW        B3,*+SP(_B3)  ;  Save for cosmetical reasons
    STW        B4,*+SP(_B4)  ;
    STW        B5,*+SP(_B5)  ;
    STW        B6,*+SP(_B6)  ;

    STW        SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;
    STW        B7 ,*+SP(_B7 ) ;
    STW        B8 ,*+SP(_B8 ) ;
    STW        B9 ,*+SP(_B9 ) ;
    STW        B10,*+SP(_B10)  ;
    STW        B11,*+SP(_B11)  ;
    STW        B12,*+SP(_B12)  ;
    STW        B13,*+SP(_B13)  ;
    STW        B14,*+SP(_B14)  ;


OSIntCtxSw_Here_Start:			; form here, the same with OSIntCtxSw

    CALL	OSTaskSwHook			; invoke the OSTaskSwHook function
    NOP		3
    MVKL	Back_Point_2,B3
    MVKH	Back_Point_2,B3

Back_Point_2:	; return here from OSTaskSwHook

    ; OSTCBCur  = OSTCBHighRdy
    ; OSPrioCur = OSPrioHighRdy
    LDW		*+DP(OSTCBHighRdy),B4
    LDB		*+DP(OSPrioHighRdy),B5
    NOP		3
    STW     B4,*+DP(OSTCBCur)
    STB     B5,*+DP(OSPrioCur)

    ; StackPointer = OSTCBHighRdy->OSTCBStkPtr
    LDW        *B4,SP
    nop    4

    ;pop current task's context from stack
    LDW		*+SP(_StartAddress),B3  ;  b3 start address
    LDW		*+SP(_IRP),B11          ;   B10 = AMR
    LDW		*+SP(_AMR),B10          ;   B10 = AMR
    LDW		*+SP(_CSR),B9           ;   B9  = CSR
;    LDW		*+SP(_IER),B8           ;   B8  = IER
    mvc		IER,B8					; do not change IER when switch tasks

    mvk     0,B13
    mvc		B13,IER

    LDW		*+SP(_A0),A0  ;
    LDW		*+SP(_A1),A1  ;
    LDW		*+SP(_A2),A2  ;
    LDW		*+SP(_A3),A3  ;
    LDW		*+SP(_A4),A4  ;
    LDW		*+SP(_A5),A5  ;
    LDW		*+SP(_A6),A6  ;
    LDW		*+SP(_A7),A7  ;
    LDW		*+SP(_A8),A8  ;
    LDW		*+SP(_A9),A9  ;
    LDW		*+SP(_A10),A10 ;
    LDW		*+SP(_A11),A11 ;
    LDW		*+SP(_A12),A12 ;
    LDW		*+SP(_A13),A13 ;
    LDW		*+SP(_A14),A14 ;
    LDW		*+SP(_A15),A15 ;

    LDW		*+SP(_B0),B0  ;
    LDW		*+SP(_B1),B1  ;
    LDW		*+SP(_B2),B2  ;

    LDW		*+SP(_B4),B4  ;
    LDW		*+SP(_B5),B5  ;
    LDW		*+SP(_B6),B6  ;
    LDW		*+SP(_B7),B7  ;

    LDW		*+SP(_B12),B12  ;
    LDW		*+SP(_B13),B13 ;
    LDW		*+SP(_B14),B14  ;

    or		3,b9,B9			; Set GIE
    mvc		B9,CSR			; set csr if IRP did not put a GIE
    mvc		B10,AMR			; Restore AMR
    mvc		B11,IRP			; Restore AMR
    LDW		*+SP(_B9),B9  ;
    LDW		*+SP(_B10),B10  ;
    LDW		*+SP(_B11),B11  ;

    LDW		*+SP(_B3),B3  ;
    LDW		*+SP(_B8),B8  ;
    LDW		*+SP(_B15),B15  ;
    B			b3             ;
    mvc			B8,IER         ;
    nop      	5


;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntCtxSw(void)
;
; Note(s): 1) Upon entry,
;             OSTCBCur     points to the OS_TCB of the task to suspend
;             OSTCBHighRdy points to the OS_TCB of the task to resume
;
;          2) The stack frame of the task to suspend
;
;          3) The stack frame of the task to resume
;
;*********************************************************************************************************

OSIntCtxSw:

    B		OSIntCtxSw_Here_Start
    NOP		5


;******************************************************************************
;* FUNCTION NAME: _Disable_int		                                          *
;*                                                                            *
;*	Notes:	function for disable interrupt                                    *
;******************************************************************************
;.align	32
Disable_int: ;
        mvc	CSR,B4
        and	1,B4,B0
 [!B0]	CLR B4,1,1,B4
 [B0]	SET B4,1,1,B4
        CLR B4,0,0,B4
        mvc B4,CSR
        B 	B3
        NOP	5


;******************************************************************************
;* FUNCTION NAME: _Enable_int: 		                                          *
;*                                                                            *
;*	Notes:	function for enable interrupt                                     *
;******************************************************************************
;.align	32
Enable_int: ;
        mvc	CSR,B4
        and	2,B4,B0
 [!B0]	CLR B4,0,0,B4
 [B0]	SET B4,0,0,B4
        mvc	B4,CSR
        B 	B3
        NOP	5


        .global OSTickISR
        .global _c_int00
;******************************************************************************
;                       Vector Table
;
; ====== vectors in use ========
;  Plug in the entry point at the ISR function in the interrupt vector table
;
; ====== unused ========
;  plug inifinite loop -- with nested branches to
;  disable interrupts -- for all undefined vectors
;
;******************************************************************************
unused  .macro id

        .global unused:id:
unused:id:
        b unused:id:    ; nested branches to block interrupts
        nop 4
        b unused:id:
        nop
        nop
        nop
        nop
        nop

        .endm

        .align  32*8*4          ; must be aligned on 256 word boundary
        .sect ".text:RESET"
RESET:                          ; reset vector
        mvkl _c_int00,b0        ; load destination function address to b0
        mvkh _c_int00,b0
        b b0                    ; start branch to destination function
        mvc PCE1,b0             ; address of interrupt vectors
        mvc b0,ISTP             ; set table to point here
        nop 3                   ; fill delay slot
        nop
        nop

                                ;------------------------------------------------
        unused 1 ;NMI           ;  plug unused interrupts with infinite loops to
        unused 2 ;Reserved      ;  catch stray interrupts
        unused 3 ;Reserved      ;------------------------------------------------

INT4:
        nop
        nop
        b OSTickISR
        nop
        nop
        nop
        nop
        nop

        unused 5 ;INT5
        unused 6 ;INT6
        unused 7 ;INT7
        unused 8 ;INT8
        unused 9 ;INT9
        unused 10 ;INT10
        unused 11 ;INT11
        unused 12 ;INT12
        unused 13 ;INT13
        unused 14 ;INT14
        unused 15 ;INT15

        .sect ".text"


;******************************************************************************
;* FUNCTION NAME: _DSP_C6x_GetCurrentDP (It's used in OSTaskStkInit function)      *
;*                                                                            *
;*   Regs Modified     : A4                                                   *
;*   Regs Used         : A4,B3                                                *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
;.align 32

DSP_C6x_GetCurrentDP:
           mv   DP,A4
           B    B3
           NOP  5

;******************************************************************************
;* FUNCTION NAME: _DSP_C6x_GetCurrentSP (It's used in any ISR function)      *
;*                                                                            *
;*   Regs Modified     : A4                                                   *
;*   Regs Used         : A4,B3                                                *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
;.align 32

DSP_C6x_GetCurrentSP:
           mv   SP,A4
           B    B3
           NOP  5

.end


