#ifndef UCOS2_C66X_SYSCALL_H
#define UCOS2_C66X_SYSCALL_H

#include <types.h>

void putc(char c);

void puts(char *str);

void putx(u32 x);

void time_delay(u32 ticks);

#endif //UCOS2_C66X_SYSCALL_H
