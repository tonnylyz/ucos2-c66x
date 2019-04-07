#include "intc.h"
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

static inline void intc_event_map(u32 eventId, u32 vectId) {
    u32 bitLow;
    if (vectId < 8) {
        bitLow = (vectId - 4U) * 8U;
        MMIO_FINSR(((intc_regs_t volatile *)INTC_REGS_BASE)->INTMUX1, bitLow + 6U, bitLow, eventId);
    } else {
        if (vectId < 12) {
            bitLow = (vectId - 8U) * 8U;
            MMIO_FINSR(((intc_regs_t volatile *)INTC_REGS_BASE)->INTMUX2, bitLow + 6U, bitLow, eventId);
        } else {
            bitLow = (vectId - 12U) * 8U;
            MMIO_FINSR(((intc_regs_t volatile *)INTC_REGS_BASE)->INTMUX3, bitLow + 6U, bitLow, eventId);
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
    val = ((intc_regs_t volatile *)INTC_REGS_BASE)->EVTMASK[y];
    previous_state = MMIO_FEXTR(val, x, x);
    MMIO_FINSR(val, x, x, 0U);
    ((intc_regs_t volatile *)INTC_REGS_BASE)->EVTMASK[y] = val;
    return previous_state;
}

void intc_event_clear(u32 eventId) {
    u32 x;
    u32 y;
    y = (eventId) >> 5U;
    x = (eventId) & 0x1fU;
    ((intc_regs_t volatile *)INTC_REGS_BASE)->EVTCLR[y] = MMIO_FMKR(x, x, 1U);
}

extern u8 core_id;

void intc_init(void) {
    intc_event_map(INTC_EVENT_TASK_TIMER, 4);
    intc_event_enable(INTC_EVENT_TASK_TIMER);

    intc_event_map(INTC_EVENT_PART_TIMER, 5);
    intc_event_enable(INTC_EVENT_PART_TIMER);

    intc_event_map(INTC_EVENT_XMC, 6);
    intc_event_enable(INTC_EVENT_XMC);

    if (core_id == 0) {
        /* Core 0 is the Mailbox 2 User 0
         * Core 1 is the Mailbox 1 User 0
         *
         * Core 0 remap  Mailbox_2_User_0 to IRQ 52 via crossbar
         * Core 1 mapped Mailbox_1_User_0 to IRQ 52 by default
         * */
        mmio_writeb(0x4a002970, 0xed);
    }

    intc_event_map(INTC_EVENT_MAILBOX, 7);
    intc_event_enable(INTC_EVENT_MAILBOX);
}