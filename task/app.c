#include <ucos_ii.h>
#include "syscall.h"

//#pragma SET_CODE_SECTION(".text:TASK")

void MyTask(void *p_arg) {
    char *sTaskName = (char *) p_arg;
    while (1) {
        task_puts(sTaskName);
        task_putc('\n');
    }
}
