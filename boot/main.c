#include <mmio.h>
#include <printf.h>
#include <timer.h>
#include <xmc.h>
#include <intc.h>
#include <os_cpu.h>
#include <ucos_ii.h>

/*
#define DSP2_PRM_BASE                (0x4AE07B00)
#define DSP2_BOOTADDR                (0x4A002560)
#define DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK	(0x3FFFFF << 0)

void dsp2_start_core() {
    u32 boot_reg;

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

u32 core_id;
*/

extern void task_entry(void *arg);

extern u32 t1_stack[];
extern u32 t2_stack[];
extern char t1_arg[];
extern char t2_arg[];

int main() {
    //core_id = CPURegisterDNUM();


    intc_init();
    printf("intc_init done\n");

    xmc_init();
    printf("xmc_init done\n");

    OSInit();
    printf("OSInit done\n");

    OSTaskCreate(task_entry, t1_arg, (void *) &t1_stack[512 - 1], 10);

    OSTaskCreate(task_entry, t2_arg, (void *) &t2_stack[512 - 1], 11);
    printf("OSTaskCreate done\n");

    timer_init();
    printf("timer_init done\n");
    OSStart();
    while (1) {
        __asm ("\tNOP");
    }
}
