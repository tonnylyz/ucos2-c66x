#include <printf.h>
#include "spinlock.h"
#include "mmio.h"

#define SPINLOCK_LOCK_REG 0x4A0F6800U
#define SPINLOCK_NUMBER 256

void spinlock_unlock(u8 id) {
    mmio_write(SPINLOCK_LOCK_REG + 4 * id, 0);
}

void spinlock_lock(u8 id) {
    do {
        __asm ("\tNOP");
    } while (mmio_read(SPINLOCK_LOCK_REG + 4 * id) != 0);
}
