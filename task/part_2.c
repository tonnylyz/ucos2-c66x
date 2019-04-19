#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p2_task_entry, ".text:PART_2")
#pragma DATA_SECTION(p2_stack, ".data:PART_2")
#pragma DATA_SECTION(p2t1_arg, ".data:PART_2")
#pragma DATA_SECTION(p2t2_arg, ".data:PART_2")
#pragma DATA_SECTION(p2t3_arg, ".data:PART_2")
#pragma DATA_SECTION(p2t4_arg, ".data:PART_2")

#define P2_STACK_SIZE 16384
u8 p2_stack[P2_STACK_SIZE];

char p2t1_arg[] = "p2t1";
char p2t2_arg[] = "p2t2";
char p2t3_arg[] = "p2t3";
char p2t4_arg[] = "p2t4";

void p2_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P2_TASK_NUM 4
#pragma DATA_SECTION(p2_task_conf_list, ".data:PART_S")
process_attribute_t p2_task_conf_list[P2_TASK_NUM] = {
        {
                .name = "p2t1",
                .entry_point = (u32) p2_task_entry,
                .stack_size = 2048,
                .arg = p2t1_arg,
                .base_priority = 11
        },
        {
                .name = "p2t2",
                .entry_point = (u32) p2_task_entry,
                .stack_size = 2048,
                .arg = p2t2_arg,
                .base_priority = 12
        },
        {
                .name = "p2t3",
                .entry_point = (u32) p2_task_entry,
                .stack_size = 2048,
                .arg = p2t3_arg,
                .base_priority = 13
        },
        {
                .name = "p2t4",
                .entry_point = (u32) p2_task_entry,
                .stack_size = 2048,
                .arg = p2t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p2_port_conf_list, ".data:PART_S")
port_conf_t p2_port_conf_list[0];

#pragma DATA_SECTION(p2_conf, ".data:PART_S")
partition_conf_t p2_conf = {
        .identifier = 2,
        .memory_addr = 0x95500000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p2_stack,
        .stack_size = P2_STACK_SIZE,
        .task_num = P2_TASK_NUM,
        .task_conf_list = p2_task_conf_list,
        .slice_num = 5,
        .target_core = 0,
        .port_num = 0,
        .port_conf_list = p2_port_conf_list,
};
