#include "syscall.h"

#pragma SET_CODE_SECTION(".text:USER")
#pragma SET_DATA_SECTION(".data:USER")

extern u32 task_syscall(u32 a0, u32 a1, u32 a2, u32 a3,
                          u32 a4, u32 a5, u32 a6, u32 a7, u32 no);

void putc(char c) {
    task_syscall((u32) c, 0, 0, 0, 0, 0, 0, 0, 0);
}

void puts(char *str) {
    u32 i;
    for (i = 0; i < 128; i++) {
        if (str[i] == 0) {
            break;
        }
        putc(str[i]);
    }
}

void putx(u32 x) {
    task_syscall(x, 0, 0, 0, 0, 0, 0, 0, 1);
}

void time_delay(u32 ticks) {
    task_syscall(ticks, 0, 0, 0, 0, 0, 0, 0, 2);
}
