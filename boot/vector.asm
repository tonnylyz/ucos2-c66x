    .def VECTOR
    .global _c_int00
    .global OSTickISR
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
    CALLDISP 1
VEC_RSV2:
    RESV 8
VEC_RSV3:
    RESV 8
VEC_INT4:
    B INT4_HANDLER
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


INT4_HANDLER:
    B OSTickISR
    NOP 3
    NOP 5
    NOP 5
    NOP 5
    NOP 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                 Context Save & Restore                       ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Reference platform/c66x/os_cpu.h

TF_ELR          .set 4
TF_A0           .set 8
TF_A1           .set 12
TF_A2           .set 16
TF_A3           .set 20
TF_A4           .set 24
TF_A5           .set 28
TF_A6           .set 32
TF_A7           .set 36
TF_A8           .set 40
TF_A9           .set 44
TF_A10          .set 48
TF_A11          .set 52
TF_A12          .set 56
TF_A13          .set 60
TF_A14          .set 64
TF_A15          .set 68
TF_B0           .set 72
TF_B1           .set 76
TF_B2           .set 80
TF_B3           .set 84
TF_B4           .set 88
TF_B5           .set 92
TF_B6           .set 96
TF_B7           .set 100
TF_B8           .set 104
TF_B9           .set 108
TF_B10          .set 112
TF_B11          .set 116
TF_B12          .set 120
TF_B13          .set 124
TF_B14          .set 128
TF_B15          .set 132
TF_AMR          .set 136
TF_CSR          .set 140
TF_IER          .set 144
TF_IRP          .set 148
TF_DUMMY        .set 152
TF_Framesize    .set 152

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .sect ".text"
    .global ContextSave
;ContextSave .macro
ContextSave:
    addk    (8-TF_Framesize),B15   ; Push one Frame

    STW     B0,*+B15(TF_B0)  ;         ; Save B0
||  mvc     AMR,B0                         ; B0 = AMR
    STW     B1,*+B15(TF_B1)  ;         ; Save B1
||  mvc     CSR,B1                         ; B1 = CSR
    STW     B2,*+B15(TF_B2)  ;         ; Save B2
||  mvc     IER,B2                         ; B2 = IER

    STW     B0,*+B15(TF_AMR)  ;        ; Save AMR
    STW     B1,*+B15(TF_CSR)  ;        ; Save CSR
    STW     B2,*+B15(TF_IER)  ;        ; Save IER

    STW     B4,*+B15(TF_B4)  ;         ; Save B4
||  mvc     IRP,B4              ; The return address in the interrupted environment
    STW     B4,*+B15 (TF_ELR)    ;  Save return address in interrupt handler start address
    STW     B4,*+B15 (TF_IRP)
    STW     B5,*+B15(TF_B5)              ; Save B5

    STW     B6,*+B15(TF_B6)  ;
    STW     B7 ,*+B15(TF_B7 ) ;
    STW     B8 ,*+B15(TF_B8 ) ;
    STW     B9 ,*+B15(TF_B9 ) ;
    STW     B10,*+B15(TF_B10)  ;
    STW     B11,*+B15(TF_B11)  ;
    STW     B12,*+B15(TF_B12)  ;
    STW     B13,*+B15(TF_B13)  ;
    STW     B14,*+B15(TF_B14)  ;

    STW     A0,*+B15(TF_A0)     ; Save A0

    LDW     *+B15(TF_DUMMY),A0  ;Get the outside B3
    STW     A1,*+B15(TF_A1)     ; Save A1
    STW     A2,*+B15(TF_A2)     ; Save A2
    STW     A3,*+B15(TF_A3)     ; Save A3
    STW     A4,*+B15(TF_A4)     ; Save A4

    STW     A0,*+B15(TF_B3)     ; Save B3
    STW     A5,*+B15(TF_A5)  ;
    STW     A6,*+B15(TF_A6)  ;
    STW     A7,*+B15(TF_A7)  ;
    STW     A8,*+B15(TF_A8)  ;
    STW     A9,*+B15(TF_A9)  ;

||  MV      B15,A0         ; A0 = B15
    STW     A10,*+B15(TF_A10) ;
||  addk    (TF_Framesize),A0      ; A0 = B15 + FRAME Correct B15 for POP of TCB
    STW     A0,*+B15(TF_B15) ;  B15 = A0 = B15 + FRAME

||  B       b3            ;  Branch to continue in the interrupt handler
    STW     A11,*+B15(TF_A11) ;
    STW     A12,*+B15(TF_A12)  ;
    STW     A13,*+B15(TF_A13)  ;
    STW     A14,*+B15(TF_A14)  ;
    STW     A15,*+B15(TF_A15)  ;

;    .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;ContextRestore .macro
    .global ContextRestore
ContextRestore:
    LDW     *+B15(TF_ELR),B3  ;  b3 start address
    LDW     *+B15(TF_AMR),B10          ;   B10 = AMR
    LDW     *+B15(TF_CSR),B9           ;   B9  = CSR
    LDW     *+B15(TF_IER),B8           ;   B8  = IER
    LDW     *+B15(TF_IRP),B7           ;   B7  = IRP

    mvk     0,B13
    mvc     B13,IER

    LDW     *+B15(TF_A0),A0  ;
    LDW     *+B15(TF_A1),A1  ;
    LDW     *+B15(TF_A2),A2  ;
    LDW     *+B15(TF_A3),A3  ;
    LDW     *+B15(TF_A4),A4  ;
    LDW     *+B15(TF_A5),A5  ;
    LDW     *+B15(TF_A6),A6  ;
    LDW     *+B15(TF_A7),A7  ;
    LDW     *+B15(TF_A8),A8  ;
    LDW     *+B15(TF_A9),A9  ;
    LDW     *+B15(TF_A10),A10 ;
    LDW     *+B15(TF_A11),A11 ;
    LDW     *+B15(TF_A12),A12 ;
    LDW     *+B15(TF_A13),A13 ;
    LDW     *+B15(TF_A14),A14 ;
    LDW     *+B15(TF_A15),A15 ;

    LDW     *+B15(TF_B0),B0  ;
    LDW     *+B15(TF_B1),B1  ;
    LDW     *+B15(TF_B2),B2  ;

    LDW     *+B15(TF_B4),B4  ;
    LDW     *+B15(TF_B5),B5  ;
    LDW     *+B15(TF_B6),B6  ;

    LDW     *+B15(TF_B11),B11  ;
    LDW     *+B15(TF_B12),B12  ;
    LDW     *+B15(TF_B13),B13 ;
    LDW     *+B15(TF_B14),B14  ;

    mvc     B7,IRP              ; Restore IRP
    or      3,b9,B9             ; Set GIE
    mvc     B9,CSR              ; set csr if IRP did not put a GIE
    mvc     B10,AMR             ; Restore AMR
    LDW     *+B15(TF_B7),B7  ;
    LDW     *+B15(TF_B9),B9  ;
    LDW     *+B15(TF_B10),B10  ;

    LDW     *+B15(TF_B3),B3  ;
    LDW     *+B15(TF_B8),B8  ;
    LDW     *+B15(TF_B15),B15  ;
    B           B3             ;
    mvc         B8,IER         ;
    nop         5

;   .endm

