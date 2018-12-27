#include "intc.h"
#include <types.h>
#include <printf.h>
#include <os_cpu.h>
#include <timer.h>
#include <ucos_ii.h>
#include <mmio.h>

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

void intc_event_clear(u32 eventId) {
    u32 x;
    u32 y;
    y = (eventId) >> 5U;
    x = (eventId) & 0x1fU;
    intc_instance->EVTCLR[y] = MMIO_FMKR(x, x, 1U);
}


void intc_init() {
    intc_event_map(INTC_EVENT_TASK_TIMER, 4);
    intc_event_enable(INTC_EVENT_TASK_TIMER);

    intc_event_map(INTC_EVENT_PART_TIMER, 5);
    intc_event_enable(INTC_EVENT_PART_TIMER);
}
