#include "syscall.h"

#pragma SET_CODE_SECTION(".text:PART_S")
#pragma SET_DATA_SECTION(".data:PART_S")

extern u32 task_syscall(u32 a0, u32 a1, u32 a2, u32 a3,
                          u32 a4, u32 a5, u32 a6, u32 a7, u32 no);

void task_putc(char c) {
    task_syscall((u32) c, 0, 0, 0, 0, 0, 0, 0, 0);
}

void task_puts(char *str) {
    task_syscall((u32) str, 0, 0, 0, 0, 0, 0, 0, 1);
}

void task_time_delay(u32 ticks) {
    task_syscall(ticks, 0, 0, 0, 0, 0, 0, 0, 2);
}
