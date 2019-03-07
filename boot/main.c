#include <mmio.h>
#include <printf.h>
#include <xmc.h>
#include <intc.h>
#include <partition.h>
#include <os_cpu.h>
#include <spinlock.h>

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

#pragma DATA_SECTION(counter, ".data:KERN_SHARE")
volatile static u32 counter = 0;

int main() {
    core_id = CPURegisterDNUM();
    mmio_write(0x01840040, 0); // disable l1d cache
    mmio_write(0x01845044, 1); // l1d cache write back invalid
    //xmc_mem_remap();
    if (core_id == 0) {
        printf("DSP_1 OS Build: %s %s\n", __DATE__, __TIME__);


        intc_init();
        printf("intc_init done\n");

        xmc_init();
        printf("xmc_init done\n");

        partition_init();
        printf("partition_init done\n");

        extern partition_conf_t p0_conf;
        extern partition_conf_t p1_conf;
        extern partition_conf_t p2_conf;
        extern partition_conf_t p3_conf;
        partition_add(&p0_conf);
        partition_add(&p1_conf);
        partition_add(&p2_conf);
        partition_add(&p3_conf);

        printf("p0 @core %d\n", pcb_list[0].target_core);
        printf("p1 @core %d\n", pcb_list[1].target_core);
        printf("p2 @core %d\n", pcb_list[2].target_core);
        printf("p3 @core %d\n", pcb_list[3].target_core);
        printf("ready to partition_start\n");

        dsp2_start_core();

        spinlock_init();

        spinlock_lock(4);
        printf("counter init: %x\n", counter);
        spinlock_unlock(4);

        int i;
        for (i = 0; i < 0x1000000; i++) {
            spinlock_lock(4);
            counter++;
            xmc_invalidate_buffer();
            spinlock_unlock(4);
        }

        spinlock_lock(4);
        printf("counter end: %x\n", counter);
        spinlock_unlock(4);

        partition_start();


    } else {
        printf("DSP_2 OS Build: %s %s\n", __DATE__, __TIME__);

        spinlock_init();

        spinlock_lock(4);
        printf("counter init: %x\n", counter);
        spinlock_unlock(4);

        int i;
        for (i = 0; i < 0x1000000; i++) {
            spinlock_lock(4);
            counter++;
            xmc_invalidate_buffer();
            spinlock_unlock(4);
        }

        spinlock_lock(4);
        printf("counter end: %x\n", counter);
        spinlock_unlock(4);

        while (1) {
            __asm ("\tNOP");
        }

        intc_init();
        printf("intc_init done\n");

        xmc_init();
        printf("xmc_init done\n");

        partition_start();
    }


    while (1) {
        __asm ("\tNOP");
    }

}
