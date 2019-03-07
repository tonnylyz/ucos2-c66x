#ifndef UCOS2_C66X_SPINLOCK_H
#define UCOS2_C66X_SPINLOCK_H

#include "types.h"

void spinlock_init();
void spinlock_lock(u8 id);
int spinlock_try_lock(u8 id);
void spinlock_unlock(u8 id);

#endif //UCOS2_C66X_SPINLOCK_H
