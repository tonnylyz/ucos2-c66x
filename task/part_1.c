#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p1_task_entry, ".text:PART_1")
#pragma DATA_SECTION(p1_stack, ".data:PART_1")
#pragma DATA_SECTION(p1t1_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t2_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t3_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t4_arg, ".data:PART_1")

u8 p1_stack[16384]; // 16KB

char p1t1_arg[] = "p1t1";
char p1t2_arg[] = "p1t2";
char p1t3_arg[] = "p1t3";
char p1t4_arg[] = "p1t4";

void p1_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P1_TASK_NUM 4
#pragma DATA_SECTION(p1_task_conf_list, ".data:PART_S")
task_conf_t p1_task_conf_list[P1_TASK_NUM] = {
        {
                .name = "p1t1",
                .entry = p1_task_entry,
                .stack_size = 2048,
                .arg = p1t1_arg,
                .priority = 11
        },
        {
                .name = "p1t2",
                .entry = p1_task_entry,
                .stack_size = 2048,
                .arg = p1t2_arg,
                .priority = 12
        },
        {
                .name = "p1t3",
                .entry = p1_task_entry,
                .stack_size = 2048,
                .arg = p1t3_arg,
                .priority = 13
        },
        {
                .name = "p1t4",
                .entry = p1_task_entry,
                .stack_size = 2048,
                .arg = p1t4_arg,
                .priority = 14
        }
};

#pragma DATA_SECTION(p1_conf, ".data:PART_S")
partition_conf_t p1_conf = {
        .identifier = 1,
        .memory_conf = {
                .address = 0x95400000,
                .size = 0x100000,
        },
        .stack_addr = (u32) p1_stack,
        .stack_size = 10240,

        .period = 0,
        .duration = 0,
        .critical_level = 0,
        .communication_conf = {
        },
        .entry_point = 0,
        .type = pt_normal,
        .task_num = P1_TASK_NUM,
        .task_conf_list = p1_task_conf_list,
        .slice_ticks = 5,
        .target_core = 1,
};
