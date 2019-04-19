#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p5_task_entry, ".text:PART_5")
#pragma DATA_SECTION(p5_stack, ".data:PART_5")
#pragma DATA_SECTION(p5t1_arg, ".data:PART_5")
#pragma DATA_SECTION(p5t2_arg, ".data:PART_5")
#pragma DATA_SECTION(p5t3_arg, ".data:PART_5")
#pragma DATA_SECTION(p5t4_arg, ".data:PART_5")

#define P5_STACK_SIZE 16384
u8 p5_stack[P5_STACK_SIZE];

char p5t1_arg[] = "p5t1";
char p5t2_arg[] = "p5t2";
char p5t3_arg[] = "p5t3";
char p5t4_arg[] = "p5t4";

void p5_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P5_TASK_NUM 4
#pragma DATA_SECTION(p5_task_conf_list, ".data:PART_S")
process_attribute_t p5_task_conf_list[P5_TASK_NUM] = {
        {
                .name = "p5t1",
                .entry_point = (u32) p5_task_entry,
                .stack_size = 2048,
                .arg = p5t1_arg,
                .base_priority = 11
        },
        {
                .name = "p5t2",
                .entry_point = (u32) p5_task_entry,
                .stack_size = 2048,
                .arg = p5t2_arg,
                .base_priority = 12
        },
        {
                .name = "p5t3",
                .entry_point = (u32) p5_task_entry,
                .stack_size = 2048,
                .arg = p5t3_arg,
                .base_priority = 13
        },
        {
                .name = "p5t4",
                .entry_point = (u32) p5_task_entry,
                .stack_size = 2048,
                .arg = p5t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p5_port_conf_list, ".data:PART_S")
port_conf_t p5_port_conf_list[0];

#pragma DATA_SECTION(p5_conf, ".data:PART_S")
partition_conf_t p5_conf = {
        .identifier = 5,
        .memory_addr = 0x95800000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p5_stack,
        .stack_size = P5_STACK_SIZE,
        .task_num = P5_TASK_NUM,
        .task_conf_list = p5_task_conf_list,
        .slice_num = 5,
        .target_core = 1,
        .port_num = 0,
        .port_conf_list = p5_port_conf_list,
};
