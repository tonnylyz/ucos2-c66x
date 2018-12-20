#include "intc.h"
#include <types.h>
#include <snprintf.h>
#include <os_cpu.h>
#include <timer.h>
#include <ucos_ii.h>
#include <mmio.h>

typedef struct {
    u32 currentVectId;
    void (*resv0)(void);
    void (*nmiIsr)(void);
    void (*resv2)(void);
    void (*resv3)(void);
    void (*isr4)(void);
    void (*isr5)(void);
    void (*isr6)(void);
    void (*isr7)(void);
    void (*isr8)(void);
    void (*isr9)(void);
    void (*isr10)(void);
    void (*isr11)(void);
    void (*isr12)(void);
    void (*isr13)(void);
    void (*isr14)(void);
    void (*isr15)(void);
} intc_table_t;
intc_table_t intc_table;

typedef struct  {
    volatile u32 EVTFLAG[4];
    volatile u8  RSVD0[16];
    volatile u32 EVTSET[4];
    volatile u8  RSVD1[16];
    volatile u32 EVTCLR[4];
    volatile u8  RSVD2[48];
    volatile u32 EVTMASK[4];
    volatile u8  RSVD3[16];
    volatile u32 MEVTFLAG[4];
    volatile u8  RSVD4[16];
    volatile u32 EXPMASK[4];
    volatile u8  RSVD5[16];
    volatile u32 MEXPFLAG[4];
    volatile u8  RSVD6[20];
    volatile u32 INTMUX1;
    volatile u32 INTMUX2;
    volatile u32 INTMUX3;
    volatile u8  RSVD7[112];
    volatile u32 INTXSTAT;
    volatile u32 INTXCLR;
    volatile u32 INTDMASK;
} intc_regs_t;


#define INTC_REGS_BASE (0x1800000U)

static intc_regs_t volatile *intc_instance = (intc_regs_t *)INTC_REGS_BASE;


void NMIHandler() {
    printf("NMIHandler called\n");
}

void INTHandler() {
    printf("INTHandler called\n");
    printf("current vect id %d\n", intc_table.currentVectId);
}

static void intc_table_init() {
    intc_table.nmiIsr = &NMIHandler;
    intc_table.isr4 = &INTHandler;
    intc_table.isr5 = &INTHandler;
    intc_table.isr6 = &INTHandler;
    intc_table.isr7 = &INTHandler;
    intc_table.isr8 = &INTHandler;
    intc_table.isr9 = &INTHandler;
    intc_table.isr10 = &INTHandler;
    intc_table.isr11 = &INTHandler;
    intc_table.isr12 = &INTHandler;
    intc_table.isr13 = &INTHandler;
    intc_table.isr14 = &INTHandler;
    intc_table.isr15 = &INTHandler;
}

void intc_init() {
    u32 prev_status;
    intc_table_init();
    intc_global_nmi_enable();
    intc_global_enable(&prev_status);
}

static inline void intc_event_map(u32 eventId, u32 vectId) {
    u32 bitLow;
    if (vectId < 8) {
        bitLow = (vectId - 4U) * 8U;
        MMIO_FINSR(intc_instance->INTMUX1, bitLow + 6U, bitLow, eventId);
    } else {
        if (vectId < 12) {
            bitLow = (vectId - 8U) * 8U;
            MMIO_FINSR(intc_instance->INTMUX2, bitLow + 6U, bitLow, eventId);
        } else {
            bitLow = (vectId - 12U) * 8U;
            MMIO_FINSR(intc_instance->INTMUX3, bitLow + 6U, bitLow, eventId);
        }
    }
}

static inline u32 intc_event_enable(u32 eventId) {
    u32 x;
    u32 y;
    u32 val;
    u32 previous_state;

    y = eventId >> 5U;
    x = eventId & 0x1fU;
    val = intc_instance->EVTMASK[y];
    previous_state = MMIO_FEXTR(val, x, x);
    MMIO_FINSR(val, x, x, 0U);
    intc_instance->EVTMASK[y] = val;
    return previous_state;
}

static inline void intc_event_clear(u32 eventId) {
    u32 x;
    u32 y;
    y = (eventId) >> 5U;
    x = (eventId) & 0x1fU;
    intc_instance->EVTCLR[y] = MMIO_FMKR(x, x, 1U);
}

#define INTC_EVENT_TIMER 67

void irq_init() {
    intc_event_map(INTC_EVENT_TIMER, 4);
    intc_event_enable(INTC_EVENT_TIMER);
    intc_enable_interrupt(4);
}

void irq_clear() {
    intc_event_clear(INTC_EVENT_TIMER);
}