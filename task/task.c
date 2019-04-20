#include <syscall.h>

#pragma CODE_SECTION(task_entry, ".text:USER")
void task_entry(void *arg) {
    while (1) {
        puts((char *)arg);
        if (((char *)arg)[5] == '2') {
            *((u32 volatile *)0x95000000) = 0x123;
        }
        putc('\n');
        time_delay(2);
    }
}

#pragma DATA_SECTION(t1_stack, ".data:USER")
#pragma DATA_SECTION(t2_stack, ".data:USER")
u32 t1_stack[512];
u32 t2_stack[512];

#pragma DATA_SECTION(t1_arg, ".data:USER")
#pragma DATA_SECTION(t2_arg, ".data:USER")
char t1_arg[] = "Task 1";
char t2_arg[] = "Task 2";
