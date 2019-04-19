#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p3_task_entry, ".text:PART_3")
#pragma DATA_SECTION(p3_stack, ".data:PART_3")
#pragma DATA_SECTION(p3t1_arg, ".data:PART_3")
#pragma DATA_SECTION(p3t2_arg, ".data:PART_3")
#pragma DATA_SECTION(p3t3_arg, ".data:PART_3")
#pragma DATA_SECTION(p3t4_arg, ".data:PART_3")

#define P3_STACK_SIZE 16384
u8 p3_stack[P3_STACK_SIZE];

char p3t1_arg[] = "p3t1";
char p3t2_arg[] = "p3t2";
char p3t3_arg[] = "p3t3";
char p3t4_arg[] = "p3t4";

void p3_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P3_TASK_NUM 4
#pragma DATA_SECTION(p3_task_conf_list, ".data:PART_S")
process_attribute_t p3_task_conf_list[P3_TASK_NUM] = {
        {
                .name = "p3t1",
                .entry_point = (u32) p3_task_entry,
                .stack_size = 2048,
                .arg = p3t1_arg,
                .base_priority = 11
        },
        {
                .name = "p3t2",
                .entry_point = (u32) p3_task_entry,
                .stack_size = 2048,
                .arg = p3t2_arg,
                .base_priority = 12
        },
        {
                .name = "p3t3",
                .entry_point = (u32) p3_task_entry,
                .stack_size = 2048,
                .arg = p3t3_arg,
                .base_priority = 13
        },
        {
                .name = "p3t4",
                .entry_point = (u32) p3_task_entry,
                .stack_size = 2048,
                .arg = p3t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p3_port_conf_list, ".data:PART_S")
port_conf_t p3_port_conf_list[0];

#pragma DATA_SECTION(p3_conf, ".data:PART_S")
partition_conf_t p3_conf = {
        .identifier = 3,
        .memory_addr = 0x95600000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p3_stack,
        .stack_size = P3_STACK_SIZE,
        .task_num = P3_TASK_NUM,
        .task_conf_list = p3_task_conf_list,
        .slice_num = 5,
        .target_core = 0,
        .port_num = 0,
        .port_conf_list = p3_port_conf_list,
};
