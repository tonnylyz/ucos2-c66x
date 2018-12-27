#include "timer.h"
#include "uart.h"
#include <types.h>
#include <mmio.h>

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


static void _timer_init(u32 base, u32 interval) {

    unsigned int load_val = 0xffffffffU - interval;

    pend_write(GP_TIMER_TLDR(base), GP_TIMER_TLDR_PEND, load_val);
    pend_write(GP_TIMER_TCRR(base), GP_TIMER_TCRR_PEND, load_val);
    pend_write(GP_TIMER_TCLR(base), GP_TIMER_TCLR_PEND, GP_TIMER_TCLR_VAL_AR_ST);

    pend_write(GP_TIMER_IRQSTATUS_SET(base), 0, GP_TIMER_INT_OVERFLOW);
}

void timer_init() {
    _timer_init(GP_TASK_TIMER_BASE, GP_TIMER_INTERVAL_10MS);
    _timer_init(GP_PART_TIMER_BASE, GP_TIMER_INTERVAL_1S);
}

static void _timer_irq_clear(u32 base) {
    mmio_write(GP_TIMER_IRQSTATUS(base), 2);
}

void timer_irq_clear(u32 base) {
    _timer_irq_clear(base);
}
