#include <ucos_ii.h>
#include "syscall.h"

#pragma SET_CODE_SECTION(".text:TASK")

#pragma SET_DATA_SECTION(".data:TASK")
void MyTask(void *p_arg) {
    char *sTaskName = (char *) p_arg;
    while (1) {
        task_puts("Name: ");
        task_puts(sTaskName);
        task_putc('\n');
        task_time_delay(10);
    }
}
