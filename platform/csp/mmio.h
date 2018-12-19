#ifndef UCOS2_C66X_MMIO_H
#define UCOS2_C66X_MMIO_H

#include <types.h>

#define KADDR(_) (_)

static inline void barrier() {
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

#endif //UCOS2_C66X_MMIO_H
