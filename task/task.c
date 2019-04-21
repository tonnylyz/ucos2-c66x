#include "syscall.h"

u32 t1_stack[512];
u32 t2_stack[512];

void task_entry(void *arg) {
    while (1) {
        puts((char *) arg);
        putc('\n');
        time_delay(2);
    }
}
