//
// Created by Tonny on 3/13/2019.
//

#ifndef UCOS2_C66X_MUTEX_H
#define UCOS2_C66X_MUTEX_H

#include <types.h>
#include <spinlock.h>

typedef struct {
    u8 spinlock_id;
    u8 core_id;
} mutex_t;

extern mutex_t mutex_list[SPINLOCK_NUMBER];

void mutex_init();

mutex_t *mutex_alloc();

void mutex_lock(mutex_t *mutex);

void mutex_unlock(mutex_t *mutex);

#endif //UCOS2_C66X_MUTEX_H
