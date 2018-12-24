#include "timer.h"
#include "uart.h"
#include <types.h>
#include <mmio.h>

#define GP_TIMER_1_BASE         KADDR(0x4AE18000)
#define GP_TIMER_2_BASE         KADDR(0x48032000)
#define GP_TIMER_3_BASE         KADDR(0x48034000)
#define GP_TIMER_4_BASE         KADDR(0x48036000)
#define GP_TIMER_5_BASE         KADDR(0x48820000)
#define GP_TIMER_6_BASE         KADDR(0x48822000)
#define GP_TIMER_7_BASE         KADDR(0x48824000)
#define GP_TIMER_8_BASE         KADDR(0x48826000)
#define GP_TIMER_9_BASE         KADDR(0x4803E000)
#define GP_TIMER_10_BASE         KADDR(0x48086000)
#define GP_TIMER_11_BASE         KADDR(0x48088000)
#define GP_TIMER_12_BASE         KADDR(0x4AE20000)
#define GP_TIMER_13_BASE         KADDR(0x48828000)
#define GP_TIMER_14_BASE         KADDR(0x4882A000)
#define GP_TIMER_15_BASE         KADDR(0x4882C000)
#define GP_TIMER_16_BASE         KADDR(0x4882E000)

#define ARM_TIMER_1 GP_TIMER_2_BASE
#define DSP_1_TIMER_1 GP_TIMER_5_BASE
#define DSP_2_TIMER_1 GP_TIMER_6_BASE

#define GP_TIMER_IRQSTATUS_RAW(base)        ((base) + 0x24u)
#define GP_TIMER_IRQSTATUS(base)            ((base) + 0x28u)
#define GP_TIMER_IRQSTATUS_SET(base)        ((base) + 0x2Cu)
#define GP_TIMER_TCLR(base)                 ((base) + 0x38u)
#define GP_TIMER_TCRR(base)                 ((base) + 0x3Cu)
#define GP_TIMER_TLDR(base)                 ((base) + 0x40u)
#define GP_TIMER_TWPS(base)                 ((base) + 0x48u)

#define GP_TIMER_TCLR_PEND    0x1
#define GP_TIMER_TCRR_PEND    0x2
#define GP_TIMER_TLDR_PEND    0x4

#define GP_TIMER_TCLR_VAL_AR_ST     0x3 /* Auto Reload + Start */
#define GP_TIMER_TCLR_VAL_ST        0x1 /* Start */


#define GP_TIMER_INT_CAPTURE			(1 << 2)
#define GP_TIMER_INT_OVERFLOW			(1 << 1)
#define GP_TIMER_INT_MATCH  			(1 << 0)

static inline void pend_write(u32 reg, u32 pend, u32 value) {
    while (mmio_read(GP_TIMER_TWPS(reg & (~0xff))) & pend) {
        barrier();
    }
    mmio_write(reg, value);
}

static inline u32 pend_read(u32 reg, u32 pend) {
    while (mmio_read(GP_TIMER_TWPS(reg & (~0xff))) & pend) {
        barrier();
    }
    return mmio_read(reg);
}

static void _timer_init(u32 base) {
    const int TIMER_FREQUENCY = 0x100000;

    unsigned int load_val = 0xffffffffU - TIMER_FREQUENCY;

    pend_write(GP_TIMER_TLDR(base), GP_TIMER_TLDR_PEND, load_val);
    pend_write(GP_TIMER_TCRR(base), GP_TIMER_TCRR_PEND, load_val);
    pend_write(GP_TIMER_TCLR(base), GP_TIMER_TCLR_PEND, GP_TIMER_TCLR_VAL_AR_ST);

    pend_write(GP_TIMER_IRQSTATUS_SET(base), 0, GP_TIMER_INT_OVERFLOW);
}

void timer_init() {
#if defined(DSP_CORE_1)
    _timer_init(DSP_1_TIMER_1);
#elif defined(DSP_CORE_2)
    _timer_init(DSP_2_TIMER_1);
#endif
}

static void _timer_irq_clear(u32 base) {
    mmio_write(GP_TIMER_IRQSTATUS(base), 2);
}

void timer_irq_clear() {
    _timer_irq_clear(DSP_1_TIMER_1);
}
