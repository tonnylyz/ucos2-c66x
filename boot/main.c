#include <mmio.h>
#include <printf.h>
#include <xmc.h>
#include <intc.h>
#include <partition.h>
#include <os_cpu.h>
#include <spinlock.h>
#include <mailbox.h>

#define DSP2_PRM_BASE                (0x4AE07B00)
#define DSP2_BOOTADDR                (0x4A002560)
#define DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK	(0x3FFFFFu << 0u)

void dsp2_start_core() {
    u32 boot_reg;

    boot_reg = mmio_read(DSP2_BOOTADDR);
    boot_reg = (boot_reg & (~DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));
    boot_reg =
            (boot_reg |
             ((0x95000000u >> 10u) &
              DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));

    mmio_write(DSP2_BOOTADDR, boot_reg);

    mmio_write(DSP2_PRM_BASE + 0x10, 0x0);
    while (((mmio_read(DSP2_PRM_BASE + 0x14u) & 0x3u) != 0x3u));
}

u8 core_id;

int main() {
    core_id = CPURegisterDNUM();
    //mmio_write(0x01840040, 0); // disable l1d cache
    //mmio_write(0x01845044, 1); // l1d cache write back invalid

    if (core_id == 0) {
        printf("DSP_1 OS Build: %s %s\n", __DATE__, __TIME__);
        intc_init();
        printf("intc_init done\n");
        xmc_init();
        printf("xmc_init done\n");
        mailbox_init();
        printf("mailbox_init done\n");
        partition_init();
        printf("partition_init done\n");

        extern partition_conf_t p0_conf;
        extern partition_conf_t p1_conf;
        partition_register(&p0_conf);
        partition_register(&p1_conf);

        dsp2_start_core();
        partition_start();
    } else {
        printf("DSP_2 OS Build: %s %s\n", __DATE__, __TIME__);
        intc_init();
        printf("intc_init done\n");
        xmc_init();
        printf("xmc_init done\n");
        mailbox_init();
        printf("mailbox_init done\n");

        partition_start();
    }
    while (1) {
        asm(" NOP");
    }
}
