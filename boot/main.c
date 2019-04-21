#include <mmio.h>
#include <printf.h>
#include <timer.h>
#include <xmc.h>
#include <intc.h>
#include <os_cpu.h>
#include <ucos_ii.h>
#include <mmc.h>
#include <mbr.h>
#include <fat32.h>

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

#pragma DATA_SECTION(t1_arg, ".data:USER")
#pragma DATA_SECTION(t2_arg, ".data:USER")
char t1_arg[] = "101010101";
char t2_arg[] = "020202020";

#define TASK_PROGRAM "task.elf"
#define TASK_PROGRAM_MAX_SIZE 16384
u8 program[TASK_PROGRAM_MAX_SIZE];

extern u32 elf_load_from(u32 addr);
extern bool elf_find_symbol(char *name, u32* addr, u32 *size);

int main() {
    u32 size;
    u32 r;
    u32 entry;
    u32 addr;

    //core_id = CPURegisterDNUM();
    printf("DSP OS Build: %s %s\n", __DATE__, __TIME__);
    intc_init();
    printf("intc_init done\n");

    xmc_init();
    printf("xmc_init done\n");

    fat32_init();
    printf("fat32_init done\n");

    size = fat32_file_size(TASK_PROGRAM);
    if (size == 0) {
        panic("task file not exists\n");
    }
    if (size > TASK_PROGRAM_MAX_SIZE) {
        printf("size %d\n", size);
        panic("task file size exceeds limit\n");
    }
    r = fat32_read_to_mem(TASK_PROGRAM, program, size);
    if (r != size) {
        panic("task file corrupts\n");
    }
    entry = elf_load_from((u32) program);
    if (entry == 0) {
        panic("task elf load failed\n");
    }

    OSInit();
    printf("OSInit done\n");

    if (!elf_find_symbol("t1_stack", &addr,  &size)) {
        panic("symbol t1_stack not found\n");
    } else {
        OSTaskCreate((void (*)(void *)) entry, t1_arg, (OS_STK *) (addr + size - 8), 10);
    }

    if (!elf_find_symbol("t2_stack", &addr,  &size)) {
        panic("symbol t2_stack not found\n");
    } else {
        OSTaskCreate((void (*)(void *)) entry, t2_arg, (OS_STK *) (addr + size - 8), 11);
    }

    printf("OSTaskCreate done\n");

    timer_init();
    printf("timer_init done\n");
    OSStart();
    while (1) {
        __asm ("\tNOP");
    }
}
