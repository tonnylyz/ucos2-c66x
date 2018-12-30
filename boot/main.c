#include <ucos_ii.h>
#include <intc.h>
#include <timer.h>
#include "resource_table.h"
#include <mmio.h>
#include <xmc.h>
#include <partition.h>


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
    //dsp2_start_core();
    //printf("dsp2_start_core done.\n");

#elif defined(DSP_CORE_2)
    printf("DSP_2 OS Build: %s %s\n", __DATE__, __TIME__);
#else
#error "No core number specified."
#endif

    xmc_init();

    intc_init();
    printf("intc_init done\n");

    partition_init();
    printf("partition_init done\n");

    extern partition_conf_t p0_conf;
    extern partition_conf_t p1_conf;
    partition_add(&p0_conf);
    partition_add(&p1_conf);

    printf("ready to partition_start\n");
    partition_start();

    while (1) {
        __asm ("\tNOP");
    }

}
