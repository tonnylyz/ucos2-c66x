#ifndef UCOS2_C66X_SYSCALL_H
#define UCOS2_C66X_SYSCALL_H

#include <types.h>

void task_putc(char c);

void task_puts(char *str);

/* syscall ~ os_time.c */

void task_time_delay(u32 ticks);

int task_time_delay_hmsm(u8 h, u8 m, u8 s, u16 ms);

int task_time_delay_resume(u8 priority);

u32 task_time_get();

void task_time_set(u32 ticks);

#endif //UCOS2_C66X_SYSCALL_H
