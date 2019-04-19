#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p0_task_entry, ".text:PART_0")
#pragma DATA_SECTION(p0_stack, ".data:PART_0")
#pragma DATA_SECTION(p0t1_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t2_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t3_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t4_arg, ".data:PART_0")

#define P1_STACK_SIZE 16384
u8 p0_stack[P1_STACK_SIZE];

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
process_attribute_t p0_task_conf_list[P0_TASK_NUM] = {
        {
                .name = "p0t1",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t1_arg,
                .base_priority = 11
        },
        {
                .name = "p0t2",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t2_arg,
                .base_priority = 12
        },
        {
                .name = "p0t3",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t3_arg,
                .base_priority = 13
        },
        {
                .name = "p0t4",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p0_port_conf_list, ".data:PART_S")
port_conf_t p0_port_conf_list[0];

#pragma DATA_SECTION(p0_conf, ".data:PART_S")
partition_conf_t p0_conf = {
        .identifier = 0,
        .memory_addr = 0x95300000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p0_stack,
        .stack_size = P1_STACK_SIZE,
        .task_num = P0_TASK_NUM,
        .task_conf_list = p0_task_conf_list,
        .slice_num = 1,
        .target_core = 0,
        .port_num = 0,
        .port_conf_list = p0_port_conf_list,
};
