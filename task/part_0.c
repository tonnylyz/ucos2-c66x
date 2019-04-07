#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p0_task_entry, ".text:PART_0")
#pragma DATA_SECTION(p0_stack, ".data:PART_0")
#pragma DATA_SECTION(p0t1_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t2_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t3_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t4_arg, ".data:PART_0")

u8 p0_stack[16384]; // 16KB

char p0t1_arg[] = "p0t1";
char p0t2_arg[] = "p0t2";
char p0t3_arg[] = "p0t3";
char p0t4_arg[] = "p0t4";

void p0_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P0_TASK_NUM 4
#pragma DATA_SECTION(p0_task_conf_list, ".data:PART_S")
task_conf_t p0_task_conf_list[P0_TASK_NUM] = {
        {
                .name = "p0t1",
                .entry = p0_task_entry,
                .stack_size = 2048,
                .arg = p0t1_arg,
                .priority = 11
        },
        {
                .name = "p0t2",
                .entry = p0_task_entry,
                .stack_size = 2048,
                .arg = p0t2_arg,
                .priority = 12
        },
        {
                .name = "p0t3",
                .entry = p0_task_entry,
                .stack_size = 2048,
                .arg = p0t3_arg,
                .priority = 13
        },
        {
                .name = "p0t4",
                .entry = p0_task_entry,
                .stack_size = 2048,
                .arg = p0t4_arg,
                .priority = 14
        }
};

#pragma DATA_SECTION(p0_conf, ".data:PART_S")
partition_conf_t p0_conf = {
        .identifier = 0,
        .memory_conf = {
                .address = 0x95300000,
                .size = 0x100000,
        },
        .stack_addr = (u32) p0_stack,
        .stack_size = 10240,

        .period = 0,
        .duration = 0,
        .critical_level = 0,
        .communication_conf = {
        },
        .entry_point = 0,
        .type = pt_normal,
        .task_num = P0_TASK_NUM,
        .task_conf_list = p0_task_conf_list,
        .slice_ticks = 5,
        .target_core = 0,
};
