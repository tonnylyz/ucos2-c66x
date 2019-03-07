#include "spinlock.h"
#include "mmio.h"

#define SPINLOCK_LOCK_REG 0x4A0F6800U
#define SPINLOCK_NUMBER 256

static u8 spinlock_taken[SPINLOCK_NUMBER]; // core private

void spinlock_init() {
    int i;
    for (i = 0; i < SPINLOCK_NUMBER; i++) {
        spinlock_taken[i] = 0;
    }
}

int spinlock_try_lock(u8 id) {
    u32 r;
    u32 lock_reg;
    lock_reg = SPINLOCK_LOCK_REG + 4 * id;
    r = mmio_read(lock_reg);
    barrier();
    if (r == 0) {
        spinlock_taken[id] = 1;
        return 0;
    }
    return 1;
}

void spinlock_unlock(u8 id) {
    u32 lock_reg;
    if (!spinlock_taken[id]) {
        return; // did not take the lock
    }
    lock_reg = SPINLOCK_LOCK_REG + 4 * id;
    mmio_write(lock_reg, 0);
    barrier();
}


void spinlock_lock(u8 id) {
    do {
        __asm ("\tNOP");
    } while (spinlock_try_lock(id) == 0);
}
