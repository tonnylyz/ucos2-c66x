#ifndef UCOS2_C66X_SYSCALL_H
#define UCOS2_C66X_SYSCALL_H

#include <types.h>

void task_putc(char c);

void task_puts(char *str);

void task_time_delay(u32 ticks);

#endif //UCOS2_C66X_SYSCALL_H
