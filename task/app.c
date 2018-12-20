#include <uart.h>
#include <ucos_ii.h>
#include "app_cfg.h"
#include "snprintf.h"

extern cregister volatile unsigned int TSR;

void MyTask(void *p_arg) {
    char *sTaskName = (char *) p_arg;

    while (1) {
        printf("Name: %s Mode: %d\n", sTaskName, TSR);
        //OSTimeDly(50);

        __asm ("\tNOP 5");
        __asm ("\tNOP 5");
        __asm ("\tSWE");
        __asm ("\tNOP 5");
        __asm ("\tNOP 5");

        int j = 0xfffff;
        while (j --) {
            __asm volatile ("\tNOP 5");
        }
    }
}

#if (OS_APP_HOOKS_EN == 1u)
void App_TaskCreateHook(OS_TCB *ptcb) {
    //uart_puts("App_TaskCreateHook called\n");
}

void App_TaskDelHook(OS_TCB *ptcb) {
    uart_puts("App_TaskDelHook called\n");

}

void App_TaskIdleHook(void) {
    //uart_puts("App_TaskIdleHook called\n");
}

void App_TaskStatHook(void) {
    uart_puts("App_TaskStatHook called\n");
}

void App_TaskReturnHook(OS_TCB *ptcb) {
    uart_puts("App_TaskReturnHook called\n");

}

void App_TaskSwHook(void) {
    //uart_puts("App_TaskSwHook called\n");

    //printf("OSTCBHighRdy->OSTCBStkPtr [%08x]\n", OSTCBHighRdy->OSTCBStkPtr);
    //printf("OSTCBHighRdy->PC [%08x]\n", OSTCBHighRdy->OSTCBStkPtr[14]);
}

void App_TCBInitHook(OS_TCB *ptcb) {
    //uart_puts("App_TCBInitHook called\n");

}

void App_TimeTickHook(void) {
    //uart_puts("App_TimeTickHook called\n");

}
#endif
