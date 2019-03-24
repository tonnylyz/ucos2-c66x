;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Symbol                             ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .def VECTOR
    .global _c_int00
    .global HandlerTaskTimer
    .global HandlerException
    .global HandlerPartitionTimer
    .global HandlerXMCException
    .global HandlerMailbox

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                           Macro                              ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RESV .macro num
    .loop num
    MVKH _c_int00,A4
    .endloop
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
    B HandlerTaskTimer
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
    B HandlerPartitionTimer
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
VEC_INT6:
    B HandlerXMCException
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
VEC_INT7:
    B HandlerMailbox
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
VEC_INT8:
    RESV 8
VEC_INT9:
    RESV 8
VEC_INT10:
    RESV 8
VEC_INT11:
    RESV 8
VEC_INT12:
    RESV 8
VEC_INT13:
    RESV 8
VEC_INT14:
    RESV 8
VEC_INT15:
    RESV 8

.end