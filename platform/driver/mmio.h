#ifndef UCOS2_C66X_MMIO_H
#define UCOS2_C66X_MMIO_H

#include <types.h>

static inline void barrier(void) {
    __asm ("\tMFENCE");
}

static inline u32 mmio_read(u32 ptr) {
    return *((u32 volatile *)ptr);
}

static inline void mmio_write(u32 ptr, u32 value) {
    *((u32 volatile *)ptr) = value;
}

static inline u8 mmio_readb(u32 ptr) {
    return *((u8 volatile *)ptr);
}

static inline void mmio_writeb(u32 ptr, u8 value) {
    *((u8 volatile *)ptr) = value;
}

/* the Field MaKe (Raw) macro */
#define MMIO_FMKR(msb, lsb, val)                                             \
    (((val) & ((((u32)1U) << ((msb) - (lsb) + ((u32)1U))) - ((u32)1U))) << (lsb))

/* the Field EXTract (Raw) macro */
#define MMIO_FEXTR(reg, msb, lsb)                                            \
    (((reg) >> (lsb)) & ((((u32)1U) << ((msb) - (lsb) + ((u32)1U))) - ((u32)1U)))

/* the Field INSert (Raw) macro */
#define MMIO_FINSR(reg, msb, lsb, val)                                       \
    ((reg) = ((reg) & (~(((((u32)1U) << ((msb) - (lsb) + ((u32)1U))) - ((u32)1U)) << (lsb))))         \
    | MMIO_FMKR((msb), (lsb), (val)))


#endif //UCOS2_C66X_MMIO_H
