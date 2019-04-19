#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p4_task_entry, ".text:PART_4")
#pragma DATA_SECTION(p4_stack, ".data:PART_4")
#pragma DATA_SECTION(p4t1_arg, ".data:PART_4")
#pragma DATA_SECTION(p4t2_arg, ".data:PART_4")
#pragma DATA_SECTION(p4t3_arg, ".data:PART_4")
#pragma DATA_SECTION(p4t4_arg, ".data:PART_4")

#define P4_STACK_SIZE 16384
u8 p4_stack[P4_STACK_SIZE];

char p4t1_arg[] = "p4t1";
char p4t2_arg[] = "p4t2";
char p4t3_arg[] = "p4t3";
char p4t4_arg[] = "p4t4";

void p4_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P4_TASK_NUM 4
#pragma DATA_SECTION(p4_task_conf_list, ".data:PART_S")
process_attribute_t p4_task_conf_list[P4_TASK_NUM] = {
        {
                .name = "p4t1",
                .entry_point = (u32) p4_task_entry,
                .stack_size = 2048,
                .arg = p4t1_arg,
                .base_priority = 11
        },
        {
                .name = "p4t2",
                .entry_point = (u32) p4_task_entry,
                .stack_size = 2048,
                .arg = p4t2_arg,
                .base_priority = 12
        },
        {
                .name = "p4t3",
                .entry_point = (u32) p4_task_entry,
                .stack_size = 2048,
                .arg = p4t3_arg,
                .base_priority = 13
        },
        {
                .name = "p4t4",
                .entry_point = (u32) p4_task_entry,
                .stack_size = 2048,
                .arg = p4t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p4_port_conf_list, ".data:PART_S")
port_conf_t p4_port_conf_list[0];

#pragma DATA_SECTION(p4_conf, ".data:PART_S")
partition_conf_t p4_conf = {
        .identifier = 4,
        .memory_addr = 0x95700000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p4_stack,
        .stack_size = P4_STACK_SIZE,
        .task_num = P4_TASK_NUM,
        .task_conf_list = p4_task_conf_list,
        .slice_num = 5,
        .target_core = 1,
        .port_num = 0,
        .port_conf_list = p4_port_conf_list,
};
