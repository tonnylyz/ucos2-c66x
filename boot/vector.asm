    .def VECTOR
    .global _c_int00
    .global HandlerTimer
    .global HandlerException
    .ref intc_table

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Macro                              ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RESV .macro num
    .loop num
    mvkh _c_int00,a4
    .endloop
    .endm

INTC_PUSH .macro reg
    stw reg,*b15--[1]
    .endm

INTC_POP .macro reg
    ldw *++b15[1],reg
    .endm

CALLDISP .macro intr
    INTC_PUSH a0
    mvkl intc_table, a0
    mvkh intc_table, a0
    ldw *++a0[intr + 1], a0
    nop 2
    stw a0, *-a0[intr + 1]
    INTC_POP a0
    bnop a0, 5
    .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Vector                             ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .align  1024
    .sect ".text:VECTOR"

VECTOR:
VEC_RSV0:
    MVKL _c_int00,B0
    MVKH _c_int00,B0
    B B0
    MVC PCE1,B0
    MVC B0,ISTP
    NOP 3
    NOP 5
    NOP 5
    NOP 5
VEC_NMI:
    B HandlerException
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
VEC_RSV2:
    RESV 8
VEC_RSV3:
    RESV 8
VEC_INT4:
    B HandlerTimer
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
    NOP 5
VEC_INT5:
    CALLDISP 5
VEC_INT6:
    CALLDISP 6
VEC_INT7:
    CALLDISP 7
VEC_INT8:
    CALLDISP 8
VEC_INT9:
    CALLDISP 9
VEC_INT10:
    CALLDISP 10
VEC_INT11:
    CALLDISP 11
VEC_INT12:
    CALLDISP 12
VEC_INT13:
    CALLDISP 13
VEC_INT14:
    CALLDISP 14
VEC_INT15:
    CALLDISP 15

.end