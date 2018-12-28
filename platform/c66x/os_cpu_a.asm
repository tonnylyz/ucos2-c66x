;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    Const                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DP               .set B14
SP               .set B15

KERN_STACK       .set 0x95108000

; Ctx_* references platform/c66x/os_cpu.h : context_frame_t
Ctx_RESV         .set 0
Ctx_A0           .set 1
Ctx_A1           .set 2
Ctx_A2           .set 3
Ctx_A3           .set 4
Ctx_A4           .set 5
Ctx_A5           .set 6
Ctx_A6           .set 7
Ctx_A7           .set 8
Ctx_A8           .set 9
Ctx_A9           .set 10
Ctx_A10          .set 11
Ctx_A11          .set 12
Ctx_A12          .set 13
Ctx_A13          .set 14
Ctx_A14          .set 15
Ctx_A15          .set 16
Ctx_B0           .set 17
Ctx_B1           .set 18
Ctx_B2           .set 19
Ctx_B3           .set 20
Ctx_B4           .set 21
Ctx_B5           .set 22
Ctx_B6           .set 23
Ctx_B7           .set 24
Ctx_B8           .set 25
Ctx_B9           .set 26
Ctx_B10          .set 27
Ctx_B11          .set 28
Ctx_B12          .set 29
Ctx_B13          .set 30
Ctx_B14          .set 31
Ctx_B15          .set 32
Ctx_ELR          .set 33
Ctx_TSR          .set 34

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    Symbols                                   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    .global OSTaskSwHook
    .global OSTaskTimerISR
    .global OSPartitionTimerISR
    .global OSExceptionISR
    .ref OSTCBHighRdy
    .ref OSTCBCur
    .ref OSPrioCur
    .ref OSPrioHighRdy
    .ref OSRunning

    .ref saved_context
    .ref __TI_STATIC_BASE

    .def HandlerTaskTimer
    .def HandlerException
    .def HandlerPartitionTimer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Macro                              ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ContextSave .macro ELR,XTSR
    MVC     SP,REP

    MVKL    saved_context,SP
    MVKH    saved_context,SP

    STW     B0,*+SP[Ctx_B0]
    STW     B1,*+SP[Ctx_B1]
    STW     B2,*+SP[Ctx_B2]
    STW     B3,*+SP[Ctx_B3]
    STW     B4,*+SP[Ctx_B4]
    STW     B5,*+SP[Ctx_B5]
    STW     B6,*+SP[Ctx_B6]
    STW     B7,*+SP[Ctx_B7]
    STW     B8,*+SP[Ctx_B8]
    STW     B9,*+SP[Ctx_B9]
    STW     B10,*+SP[Ctx_B10]
    STW     B11,*+SP[Ctx_B11]
    STW     B12,*+SP[Ctx_B12]
    STW     B13,*+SP[Ctx_B13]

    MVC     REP,B0
    STW     B0,*+SP[Ctx_B15]

    STW     A0,*+SP[Ctx_A0]
    STW     A1,*+SP[Ctx_A1]
    STW     A2,*+SP[Ctx_A2]
    STW     A3,*+SP[Ctx_A3]
    STW     A4,*+SP[Ctx_A4]
    STW     A5,*+SP[Ctx_A5]
    STW     A6,*+SP[Ctx_A6]
    STW     A7,*+SP[Ctx_A7]
    STW     A8,*+SP[Ctx_A8]
    STW     A9,*+SP[Ctx_A9]
    STW     A10,*+SP[Ctx_A10]
    STW     A11,*+SP[Ctx_A11]
    STW     A12,*+SP[Ctx_A12]
    STW     A13,*+SP[Ctx_A13]
    STW     A14,*+SP[Ctx_A14]
    STW     A15,*+SP[Ctx_A15]

    MVC     ELR,B0
    MVC     XTSR,B1

    STW     B0,*+SP[Ctx_ELR]
    STW     B1,*+SP[Ctx_TSR]

    .endm

ContextRestore .macro ELR,XTSR
    MVKL    saved_context,SP
    MVKH    saved_context,SP

    LDW     *+SP[Ctx_ELR],B0
    LDW     *+SP[Ctx_TSR],B1

    LDW     *+SP[Ctx_A0],A0
    LDW     *+SP[Ctx_A1],A1
    LDW     *+SP[Ctx_A2],A2
    LDW     *+SP[Ctx_A3],A3
    LDW     *+SP[Ctx_A4],A4
    LDW     *+SP[Ctx_A5],A5
    LDW     *+SP[Ctx_A6],A6
    LDW     *+SP[Ctx_A7],A7
    LDW     *+SP[Ctx_A8],A8
    LDW     *+SP[Ctx_A9],A9
    LDW     *+SP[Ctx_A10],A10
    LDW     *+SP[Ctx_A11],A11
    LDW     *+SP[Ctx_A12],A12
    LDW     *+SP[Ctx_A13],A13
    LDW     *+SP[Ctx_A14],A14
    LDW     *+SP[Ctx_A15],A15

    ; DON'T mvc from B0 immediatelly after load (causing IXF)
    MVC     B0,ELR
    MVC     B1,XTSR

    LDW     *+SP[Ctx_B0],B0
    LDW     *+SP[Ctx_B1],B1
    LDW     *+SP[Ctx_B2],B2
    LDW     *+SP[Ctx_B3],B3
    LDW     *+SP[Ctx_B4],B4
    LDW     *+SP[Ctx_B5],B5
    LDW     *+SP[Ctx_B6],B6
    LDW     *+SP[Ctx_B7],B7
    LDW     *+SP[Ctx_B8],B8
    LDW     *+SP[Ctx_B9],B9
    LDW     *+SP[Ctx_B10],B10
    LDW     *+SP[Ctx_B11],B11
    LDW     *+SP[Ctx_B12],B12
    LDW     *+SP[Ctx_B13],B13
    LDW     *+SP[Ctx_B14],B14
    LDW     *+SP[Ctx_B15],B15

    B       ELR
    NOP     5
    .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    OSStartHighRdy                            ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global OSStartHighRdy
OSStartHighRdy:
    CALL    OSTaskSwHook
    NOP     3
    MVKL    OSStartHighRdy_1,B3
    MVKH    OSStartHighRdy_1,B3

OSStartHighRdy_1:    ;return here from OSTaskSwHook

    ; OSRunning = TRUE
    MVK     1,B1
    STB     B1,*+DP(OSRunning)

    MVK     0,B1
    MVC     B1,AMR

    MVC     CSR,B1
    OR      3,B1,B1 ; Enable all interrupts
    MVC     B1,CSR

    MVKH    0xfff3,B1 ; Enable INT4 ~ INT15, NMI
    MVKL    0xfff3,B1 ; Enable INT4 ~ INT15, NMI
    MVC     B1,IER

    ; Enable GEE bit in TSR (Cannot be loaded via ITSR)
    MVC     TSR,B1
    MVK     4,B2
    OR      B1,B2,B1
    MVC     B1,TSR

    B       IntRestore
    NOP     5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    OSCtxSw                                   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global OSCtxSw
OSCtxSw:
    CALL    OSTaskSwHook            ; invoke the OSTaskSwHook function
    NOP     3
    MVKL    OSCtxSw_1,B3
    MVKH    OSCtxSw_1,B3

OSCtxSw_1:    ; return here from OSTaskSwHook

    ; OSTCBCur  = OSTCBHighRdy
    ; OSPrioCur = OSPrioHighRdy
    LDW     *+DP(OSTCBHighRdy),B4
    LDB     *+DP(OSPrioHighRdy),B5
    NOP     3
    STW     B4,*+DP(OSTCBCur)
    STB     B5,*+DP(OSPrioCur)

    B       ExcRestore
    NOP     5


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                     Exception Handler                        ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Notice: system call also handled here
    .global HandlerException
HandlerException:
    ContextSave NRP,NTSR

    MVKL    KERN_STACK,SP
    MVKH    KERN_STACK,SP

    MVKL    __TI_STATIC_BASE,DP
    MVKH    __TI_STATIC_BASE,DP

    MVC     EFR,B0
    MV      B0,A4
    MVC     IERR,B0
    MV      B0,B4 ; OSExceptionISR(EFR,IERR)
    CALL	OSExceptionISR
    NOP		3
    MVKL	ExcRestore,B3
    MVKH	ExcRestore,B3

    ; Clear EFR.SXF
    MVK     1,B0
    MVC     B0,ECR

ExcRestore:
    ContextRestore NRP,NTSR


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    Task Timer Handler                        ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global HandlerTaskTimer
HandlerTaskTimer:
    ContextSave IRP,ITSR

    MVKL    KERN_STACK,SP
    MVKH    KERN_STACK,SP

    MVKL    __TI_STATIC_BASE,DP
    MVKH    __TI_STATIC_BASE,DP


    CALL	OSTaskTimerISR
    NOP		3
    MVKL	IntRestore,B3
    MVKH	IntRestore,B3

    ; OSTickISR can invoke OSIntCtxSw below
    ; If no context switch is needed, go straight down

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                         OSIntCtxSw                           ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global OSIntCtxSw
OSIntCtxSw:
    CALL	OSTaskSwHook
    NOP		3
    MVKL	OSIntCtxSw_1,B3
    MVKH	OSIntCtxSw_1,B3

OSIntCtxSw_1:

    ; OSTCBCur  = OSTCBHighRdy
    ; OSPrioCur = OSPrioHighRdy
    LDW		*+DP(OSTCBHighRdy),B4
    LDB		*+DP(OSPrioHighRdy),B5
    NOP		3
    STW     B4,*+DP(OSTCBCur)
    STB     B5,*+DP(OSPrioCur)

IntRestore:
    ContextRestore IRP,ITSR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                   Partition Timer Handler                    ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global HandlerPartitionTimer
HandlerPartitionTimer:
    ContextSave IRP,ITSR

    MVKL    KERN_STACK,SP
    MVKH    KERN_STACK,SP

    MVKL    __TI_STATIC_BASE,DP
    MVKH    __TI_STATIC_BASE,DP

    CALL	OSPartitionTimerISR
    NOP		3
    MVKL	IntRestore,B3
    MVKH	IntRestore,B3

.end


