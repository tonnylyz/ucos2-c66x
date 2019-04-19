#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p6_task_entry, ".text:PART_6")
#pragma DATA_SECTION(p6_stack, ".data:PART_6")
#pragma DATA_SECTION(p6t1_arg, ".data:PART_6")
#pragma DATA_SECTION(p6t2_arg, ".data:PART_6")
#pragma DATA_SECTION(p6t3_arg, ".data:PART_6")
#pragma DATA_SECTION(p6t4_arg, ".data:PART_6")

#define P6_STACK_SIZE 16384
u8 p6_stack[P6_STACK_SIZE];

char p6t1_arg[] = "p6t1";
char p6t2_arg[] = "p6t2";
char p6t3_arg[] = "p6t3";
char p6t4_arg[] = "p6t4";

void p6_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P6_TASK_NUM 4
#pragma DATA_SECTION(p6_task_conf_list, ".data:PART_S")
process_attribute_t p6_task_conf_list[P6_TASK_NUM] = {
        {
                .name = "p6t1",
                .entry_point = (u32) p6_task_entry,
                .stack_size = 2048,
                .arg = p6t1_arg,
                .base_priority = 11
        },
        {
                .name = "p6t2",
                .entry_point = (u32) p6_task_entry,
                .stack_size = 2048,
                .arg = p6t2_arg,
                .base_priority = 12
        },
        {
                .name = "p6t3",
                .entry_point = (u32) p6_task_entry,
                .stack_size = 2048,
                .arg = p6t3_arg,
                .base_priority = 13
        },
        {
                .name = "p6t4",
                .entry_point = (u32) p6_task_entry,
                .stack_size = 2048,
                .arg = p6t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p6_port_conf_list, ".data:PART_S")
port_conf_t p6_port_conf_list[0];

#pragma DATA_SECTION(p6_conf, ".data:PART_S")
partition_conf_t p6_conf = {
        .identifier = 6,
        .memory_addr = 0x95900000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p6_stack,
        .stack_size = P6_STACK_SIZE,
        .task_num = P6_TASK_NUM,
        .task_conf_list = p6_task_conf_list,
        .slice_num = 5,
        .target_core = 1,
        .port_num = 0,
        .port_conf_list = p6_port_conf_list,
};
