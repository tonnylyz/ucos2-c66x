#include "part_0.h"
#include "syscall.h"
#include <partition_conf.h>
#include <printf.h>

#pragma SET_CODE_SECTION(".text:PART_0")
#pragma SET_DATA_SECTION(".data:PART_0")

u8 p0t1_stack[P0_TASK_STACK_SIZE];
u8 p0t2_stack[P0_TASK_STACK_SIZE];
u8 p0t3_stack[P0_TASK_STACK_SIZE];
u8 p0t4_stack[P0_TASK_STACK_SIZE];
u8 p0t5_stack[P0_TASK_STACK_SIZE];

char p0t1_arg[] = "Task 1";
char p0t2_arg[] = "Task 2";
char p0t3_arg[] = "Task 3";
char p0t4_arg[] = "Task 4";
char p0t5_arg[] = "Task 5";

void p0t0_entry(void *p_arg) {
    char *sTaskName = (char *) p_arg;
    while (1) {
        task_puts("Name: ");
        task_puts(sTaskName);
        task_putc('\n');
        task_time_delay(10);
    }
}

/* Partition 0 Configuration */

task_init_t p0_tasks[5] = {
        {
                .entry = p0t0_entry,
                .stack_ptr = &p0t1_stack[P0_TASK_STACK_SIZE - 1],
                .stack_size = P0_TASK_STACK_SIZE,
                .arg = p0t1_arg,
                .priority = P0_TASK_1_PRIO
        },
        {
                .entry = p0t0_entry,
                .stack_ptr = &p0t2_stack[P0_TASK_STACK_SIZE - 1],
                .stack_size = P0_TASK_STACK_SIZE,
                .arg = p0t2_arg,
                .priority = P0_TASK_2_PRIO
        },
        {
                .entry = p0t0_entry,
                .stack_ptr = &p0t3_stack[P0_TASK_STACK_SIZE - 1],
                .stack_size = P0_TASK_STACK_SIZE,
                .arg = p0t3_arg,
                .priority = P0_TASK_3_PRIO
        },
        {
                .entry = p0t0_entry,
                .stack_ptr = &p0t4_stack[P0_TASK_STACK_SIZE - 1],
                .stack_size = P0_TASK_STACK_SIZE,
                .arg = p0t4_arg,
                .priority = P0_TASK_4_PRIO
        },
        {
                .entry = p0t0_entry,
                .stack_ptr = &p0t5_stack[P0_TASK_STACK_SIZE - 1],
                .stack_size = P0_TASK_STACK_SIZE,
                .arg = p0t5_arg,
                .priority = P0_TASK_5_PRIO
        }
};

part_init_t p0_init = {
        .memory_layout = {
                .address = 0x95300000,
                .size = 0x100000,
        },
        .task_num = 5,
        .task_init = p0_tasks
};
