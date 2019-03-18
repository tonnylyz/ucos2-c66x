#ifndef UCOS2_C66X_SPINLOCK_H
#define UCOS2_C66X_SPINLOCK_H

#include "types.h"

#define SPINLOCK_NUMBER 256

void spinlock_lock(u8 id);
void spinlock_unlock(u8 id);

#endif //UCOS2_C66X_SPINLOCK_H
