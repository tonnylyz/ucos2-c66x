#include <mmio.h>
#include <printf.h>
#include <xmc.h>
#include <intc.h>
#include <partition.h>
#include <os_cpu.h>
#include <spinlock.h>
#include <mailbox.h>
#include <timer.h>

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

bool core_ready;

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
        extern partition_conf_t p2_conf;
        extern partition_conf_t p3_conf;
        extern partition_conf_t p4_conf;
        extern partition_conf_t p5_conf;
        extern partition_conf_t p6_conf;
        extern partition_conf_t p7_conf;


        partition_register(&p0_conf);
        partition_register(&p1_conf);
        //partition_register(&p2_conf);
        //partition_register(&p3_conf);
//        partition_register(&p4_conf);
//        partition_register(&p5_conf);
//        partition_register(&p6_conf);
//        partition_register(&p7_conf);

        /*InstructionCounterStart();
        u32 l = InstructionCounterTSCL();
        u32 h = InstructionCounterTSCH();
        printf("InstructionCounterTSCL : %08x\n", l);
        printf("InstructionCounterTSCH : %08x\n", h);
        */
        /* Note:
         *  `l` = 0xd above
         *  means that these function calls' overhead is about 13 clocks
         * */

        /*timer_benchmark_restart();
        u32 j = 0x1000000;
        while (j --) {
            asm(" NOP");
        }
        u32 cost_time = timer_benchmark_stop();
        printf("cost_time ~ ns: %d\n", cost_time * 50);*/
        /* Note:
         *  Timer clock is 20MHz ~ 50ns per increment
         *  DSP clock is 600 MHz
         *  Code above is about 0x8000000 instructions
         * */

        dsp2_start_core();
        timer_init();
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
