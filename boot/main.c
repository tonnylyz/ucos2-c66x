#include <ucos_ii.h>
#include <intc.h>
#include <timer.h>
#include "resource_table.h"
#include <mmio.h>

extern void MyTask(void *p_arg);

#pragma SET_DATA_SECTION(".data:TASK")

INT8U Stk1[APP_TASK_START_STK_SIZE];
INT8U Stk2[APP_TASK_START_STK_SIZE];
INT8U Stk3[APP_TASK_START_STK_SIZE];
INT8U Stk4[APP_TASK_START_STK_SIZE];
INT8U Stk5[APP_TASK_START_STK_SIZE];

char sTask1[] = "Task 1";
char sTask2[] = "Task 2";
char sTask3[] = "Task 3";
char sTask4[] = "Task 4";
char sTask5[] = "Task 5";

#pragma SET_DATA_SECTION()


#define DSP2_PRM_BASE                (0x4AE07B00)
#define DSP2_BOOTADDR                (0x4A002560)
#define DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK	(0x3FFFFF << 0)

void dsp2_start_core() {    u32 boot_reg;

    boot_reg = mmio_read(DSP2_BOOTADDR);
    boot_reg = (boot_reg & (~DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));
    boot_reg =
            (boot_reg |
             ((0x95000000 >> 10) &
              DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));

    mmio_write(DSP2_BOOTADDR, boot_reg);

    mmio_write(DSP2_PRM_BASE + 0x10, 0x0);
    while (((mmio_read(DSP2_PRM_BASE + 0x14) & 0x3) != 0x3));

}

int main() {
#if defined(DSP_CORE_1)
    printf("DSP_1 OS Build: %s %s\n", __DATE__, __TIME__);
    dsp2_start_core();
    printf("dsp2_start_core done.\n");

#elif defined(DSP_CORE_2)
    printf("DSP_2 OS Build: %s %s\n", __DATE__, __TIME__);
#else
#error "No core number specified."
#endif
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

    printf("task create done\n");
    OSStart();
    while (1) {
        __asm ("\tNOP");
    }

}
