#include <ucos_ii.h>
#include <intc.h>
#include <timer.h>
#include "resource_table.h"

extern void MyTask(void *p_arg);

//#pragma SET_DATA_SECTION(".data:TASK")

INT8U Stk1[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk2[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk3[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk4[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk5[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));

char sTask1[] = "Task 1";
char sTask2[] = "Task 2";
char sTask3[] = "Task 3";
char sTask4[] = "Task 4";
char sTask5[] = "Task 5";

//#pragma SET_DATA_SECTION()

int main() {
    printf("DSP OS Build: %s %s\n", __DATE__, __TIME__);

    intc_init();
    printf("intc_init done\n");

    irq_init();
    printf("irq_init done\n");

    timer_init();
    printf("timer_init done\n");

    OSInit();
    printf("OSInit done\n");

    OSTaskCreate(MyTask, sTask1,
                 (void *) &Stk1[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_1_PRIO);
/*

    OSTaskCreate(MyTask, sTask2,
                 (void *) &Stk2[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_2_PRIO);
    OSTaskCreate(MyTask, sTask3,
                 (void *) &Stk3[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_3_PRIO);

    OSTaskCreate(MyTask, sTask4,
                 (void *) &Stk4[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_4_PRIO);

    OSTaskCreate(MyTask, sTask5,
                 (void *) &Stk5[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_5_PRIO);
*/

    printf("task create done\n");
    OSStart();
    while (1) {
        __asm ("\tNOP");
    }

}
