//
// Created by Tonny on 3/13/2019.
//

#include <os_cpu.h>
#include "mutex.h"


#pragma DATA_SECTION(mutex_list, ".data:KERN_SHARE")
mutex_t mutex_list[SPINLOCK_NUMBER];

void mutex_init() {
    int i;
    /* Spinlock 0 is used to protect mutex_list */
    mutex_list[0].spinlock_id = 0;
    mutex_list[0].core_id = 0xff;
    for (i = 1; i < SPINLOCK_NUMBER; i++) {
        mutex_list[i].spinlock_id = 0xff;
        mutex_list[i].core_id = 0xff;
    }
}

mutex_t *mutex_alloc() {
    int i;
    spinlock_lock(0);
    for (i = 1; i < SPINLOCK_NUMBER; i++) {
        if (mutex_list[i].spinlock_id == 0xff) {
            mutex_list[i].spinlock_id = (u8)i;
            mutex_list[i].core_id = core_id;
        }
    }
    spinlock_unlock(0);
    return NULL;
}

void mutex_lock(mutex_t *mutex) {
    spinlock_lock(mutex->spinlock_id);
}

void mutex_unlock(mutex_t *mutex) {
    spinlock_unlock(mutex->spinlock_id);
}
