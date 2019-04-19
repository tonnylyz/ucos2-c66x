#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p7_task_entry, ".text:PART_7")
#pragma DATA_SECTION(p7_stack, ".data:PART_7")
#pragma DATA_SECTION(p7t1_arg, ".data:PART_7")
#pragma DATA_SECTION(p7t2_arg, ".data:PART_7")
#pragma DATA_SECTION(p7t3_arg, ".data:PART_7")
#pragma DATA_SECTION(p7t4_arg, ".data:PART_7")

#define P7_STACK_SIZE 16384
u8 p7_stack[P7_STACK_SIZE];

char p7t1_arg[] = "p7t1";
char p7t2_arg[] = "p7t2";
char p7t3_arg[] = "p7t3";
char p7t4_arg[] = "p7t4";

void p7_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P7_TASK_NUM 4
#pragma DATA_SECTION(p7_task_conf_list, ".data:PART_S")
process_attribute_t p7_task_conf_list[P7_TASK_NUM] = {
        {
                .name = "p7t1",
                .entry_point = (u32) p7_task_entry,
                .stack_size = 2048,
                .arg = p7t1_arg,
                .base_priority = 11
        },
        {
                .name = "p7t2",
                .entry_point = (u32) p7_task_entry,
                .stack_size = 2048,
                .arg = p7t2_arg,
                .base_priority = 12
        },
        {
                .name = "p7t3",
                .entry_point = (u32) p7_task_entry,
                .stack_size = 2048,
                .arg = p7t3_arg,
                .base_priority = 13
        },
        {
                .name = "p7t4",
                .entry_point = (u32) p7_task_entry,
                .stack_size = 2048,
                .arg = p7t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p7_port_conf_list, ".data:PART_S")
port_conf_t p7_port_conf_list[0];

#pragma DATA_SECTION(p7_conf, ".data:PART_S")
partition_conf_t p7_conf = {
        .identifier = 7,
        .memory_addr = 0x95a00000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p7_stack,
        .stack_size = P7_STACK_SIZE,
        .task_num = P7_TASK_NUM,
        .task_conf_list = p7_task_conf_list,
        .slice_num = 5,
        .target_core = 1,
        .port_num = 0,
        .port_conf_list = p7_port_conf_list,
};
