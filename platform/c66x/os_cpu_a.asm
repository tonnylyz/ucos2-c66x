;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    Const                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DP               .set B14
SP               .set B15

; Ctx_* references platform/c66x/os_cpu.h : context_frame_t
Ctx_Entry        .set 0
Ctx_A0           .set 4
Ctx_A1           .set 8
Ctx_A2           .set 12
Ctx_A3           .set 16
Ctx_A4           .set 20
Ctx_A5           .set 24
Ctx_A6           .set 28
Ctx_A7           .set 32
Ctx_A8           .set 36
Ctx_A9           .set 40
Ctx_A10          .set 44
Ctx_A11          .set 48
Ctx_A12          .set 52
Ctx_A13          .set 56
Ctx_A14          .set 60
Ctx_A15          .set 64
Ctx_B0           .set 68
Ctx_B1           .set 72
Ctx_B2           .set 76
Ctx_B3           .set 80
Ctx_B4           .set 84
Ctx_B5           .set 88
Ctx_B6           .set 92
Ctx_B7           .set 96
Ctx_B8           .set 100
Ctx_B9           .set 104
Ctx_B10          .set 108
Ctx_B11          .set 112
Ctx_B12          .set 116
Ctx_B13          .set 120
Ctx_B14          .set 124
Ctx_B15          .set 128
Ctx_AMR          .set 132
Ctx_CSR          .set 136
Ctx_IER          .set 140
Ctx_ELR          .set 144
Ctx_TSR          .set 148

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                    Symbols                                   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    .global OSTaskSwHook
    .global OSTimerISR
    .global OSExceptionISR
    .ref OSTCBHighRdy
    .ref OSTCBCur
    .ref OSPrioCur
    .ref OSPrioHighRdy
    .ref OSRunning

    .ref saved_context
    .ref __TI_STACK_END
    .ref __TI_STATIC_BASE

    .def HandlerTimer
    .def HandlerException

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Macro                              ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ContextSave .macro ELR,XTSR
    MVC     SP,REP

    MVKL    saved_context,SP
    MVKH    saved_context,SP

    STW     B0,*+SP(Ctx_B0)
    STW     B1,*+SP(Ctx_B1)
    STW     B2,*+SP(Ctx_B2)
    STW     B3,*+SP(Ctx_B3)
    STW     B4,*+SP(Ctx_B4)
    STW     B5,*+SP(Ctx_B5)
    STW     B6,*+SP(Ctx_B6)
    STW     B7,*+SP(Ctx_B7)
    STW     B8,*+SP(Ctx_B8)
    STW     B9,*+SP(Ctx_B9)
    STW     B10,*+SP(Ctx_B10)
    STW     B11,*+SP(Ctx_B11)
    STW     B12,*+SP(Ctx_B12)
    STW     B13,*+SP(Ctx_B13)

    MVC     REP,B0
    STW     B0,*+SP(Ctx_B15)

    STW     A0,*+SP(Ctx_A0)
    STW     A1,*+SP(Ctx_A1)
    STW     A2,*+SP(Ctx_A2)
    STW     A3,*+SP(Ctx_A3)
    STW     A4,*+SP(Ctx_A4)
    STW     A5,*+SP(Ctx_A5)
    STW     A6,*+SP(Ctx_A6)
    STW     A7,*+SP(Ctx_A7)
    STW     A8,*+SP(Ctx_A8)
    STW     A9,*+SP(Ctx_A9)
    STW     A10,*+SP(Ctx_A10)
    STW     A11,*+SP(Ctx_A11)
    STW     A12,*+SP(Ctx_A12)
    STW     A13,*+SP(Ctx_A13)
    STW     A14,*+SP(Ctx_A14)
    STW     A15,*+SP(Ctx_A15)

    ;MVC     AMR,B7
    MVC     CSR,B8
    MVC     IER,B9
    MVC     ELR,B10
    MVC     XTSR,B11

    STW     B7,*+SP(Ctx_AMR)
    STW     B8,*+SP(Ctx_CSR)
    STW     B9,*+SP(Ctx_IER)
    STW     B10,*+SP(Ctx_ELR)
    STW     B11,*+SP(Ctx_TSR)

    .endm

ContextRestore .macro ELR,XTSR
    MVKL    saved_context,SP
    MVKH    saved_context,SP

    LDW     *+SP(Ctx_ELR),B7
    LDW     *+SP(Ctx_IER),B8
    LDW     *+SP(Ctx_CSR),B9
    LDW     *+SP(Ctx_AMR),B10
    LDW     *+SP(Ctx_TSR),B11

    LDW     *+SP(Ctx_A0),A0
    LDW     *+SP(Ctx_A1),A1
    LDW     *+SP(Ctx_A2),A2
    LDW     *+SP(Ctx_A3),A3
    LDW     *+SP(Ctx_A4),A4
    LDW     *+SP(Ctx_A5),A5
    LDW     *+SP(Ctx_A6),A6
    LDW     *+SP(Ctx_A7),A7
    LDW     *+SP(Ctx_A8),A8
    LDW     *+SP(Ctx_A9),A9
    LDW     *+SP(Ctx_A10),A10
    LDW     *+SP(Ctx_A11),A11
    LDW     *+SP(Ctx_A12),A12
    LDW     *+SP(Ctx_A13),A13
    LDW     *+SP(Ctx_A14),A14
    LDW     *+SP(Ctx_A15),A15

    LDW     *+SP(Ctx_B0),B0
    LDW     *+SP(Ctx_B1),B1
    LDW     *+SP(Ctx_B2),B2
    LDW     *+SP(Ctx_B3),B3
    LDW     *+SP(Ctx_B4),B4
    LDW     *+SP(Ctx_B5),B5
    LDW     *+SP(Ctx_B6),B6

    LDW     *+SP(Ctx_B12),B12
    LDW     *+SP(Ctx_B13),B13
    LDW     *+SP(Ctx_B14),B14

    MVC     B7,ELR
    MVC     B8,IER
    MVC     B9,CSR
    MVC     B10,AMR
    MVC     B11,NTSR

    LDW     *+SP(Ctx_B7),B7
    LDW     *+SP(Ctx_B8),B8
    LDW     *+SP(Ctx_B9),B9
    LDW     *+SP(Ctx_B10),B10
    LDW     *+SP(Ctx_B11),B11
    LDW     *+SP(Ctx_B15),B15

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

    ; Enable GEE bit in TSR (Cannot be loaded via ITSR)
    MVC     TSR,B1
    MVK     4,B2
    OR      B1,B2,B1
    MVC     B1,TSR

    ContextRestore IRP,ITSR

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
    MVKL    __TI_STACK_END,SP
    MVKH    __TI_STACK_END,SP

    MVC     EFR,B0
    MV      B0,A4 ; OSExceptionISR(EFR)
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
;;;                         Timer Handler                        ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .global HandlerTimer
HandlerTimer:
    ContextSave IRP,ITSR
    MVKL    __TI_STACK_END,SP
    MVKH    __TI_STACK_END,SP

    CALL	OSTimerISR
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

.end


