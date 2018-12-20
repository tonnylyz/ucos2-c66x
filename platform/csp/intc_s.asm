INTC_GIE_MASK .equ 0x00000001
INTC_GIE_MASK_DISABLE .equ 0xfffffffe

    .global intc_global_nmi_enable           
intc_global_nmi_enable:
    bnop b3, 1
    mvc ier, b0
    or 2, b0, b0
    mvc b0, ier
    mvk 1, a4

    .global intc_global_enable
intc_global_enable:
    bnop b3
    mv a4,a0
    ||mvc csr, b0
    and b0 , INTC_GIE_MASK, a3
 [a0]  stnw a3 , *a0
    ||or  b0 , INTC_GIE_MASK, b0   
    mvc b0 , csr
    mvk 1, a4

    .global intc_vector_enable
intc_vector_enable:
    ;input argument is in a4
    ;return value is in a4
    bnop b3, 1
    mvk   1, a5
    mvc ier, b0
    ||shl  a5, a4, a5 ; Or mask obtained
    shru b0, a5, a4 ; return value obtained
    ||or   a5, b0, b0
    mvc  b0, ier
    ||and a4,1,a4
   
    .global intc_vector_disable
intc_vector_disable:
    ;input argument is in a4
    ;return value is in a4
    bnop b3, 1
    mvk   1, a5
    mvc ier, b0
    ||shl  a5, a4, a5 ; Or mask obtained
    shru b0, a4, a4 ; return value obtained
    ||andn  b0, a5, b0
    mvc  b0, ier
    ||and a4,1,a4

