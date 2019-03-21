#include "part_3.h"

#include <syscall.h>
#include <types.h>
#include <os_cfg.h>
#include <partition_conf.h>
#include <ucos_ii.h>

#pragma SET_CODE_SECTION(".text:PART_3")
#pragma SET_DATA_SECTION(".data:PART_3")

u8 p3t1_stack[P3_TASK_STACK_SIZE];
u8 p3t2_stack[P3_TASK_STACK_SIZE];
u8 p3t3_stack[P3_TASK_STACK_SIZE];
u8 p3t4_stack[P3_TASK_STACK_SIZE];
u8 p3_idle_stack[OS_TASK_IDLE_STK_SIZE];

char p3t1_arg[] = "p3t1";
char p3t2_arg[] = "p3t2";
char p3t3_arg[] = "p3t3";
char p3t4_arg[] = "p3t4";

void p3t0_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

void p3_idle_entry(void *arg) {
    while (1) {
        asm volatile ("\tNOP 5");
    }
}

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
/* Partition 3 Configuration */

task_conf_t p3_tasks[5] = {
        {
                .entry = p3_idle_entry,
                .stack_ptr = &p3_idle_stack[OS_TASK_IDLE_STK_SIZE - 1],
                .stack_size = OS_TASK_IDLE_STK_SIZE,
                .arg = NULL,
                .priority = OS_TASK_IDLE_PRIO
        },
        {
                .entry = p3t0_entry,
                .stack_ptr = &p3t1_stack[P3_TASK_STACK_SIZE - 1],
                .stack_size = P3_TASK_STACK_SIZE,
                .arg = p3t1_arg,
                .priority = P3_TASK_1_PRIO
        },
        {
                .entry = p3t0_entry,
                .stack_ptr = &p3t2_stack[P3_TASK_STACK_SIZE - 1],
                .stack_size = P3_TASK_STACK_SIZE,
                .arg = p3t2_arg,
                .priority = P3_TASK_2_PRIO
        },
        {
                .entry = p3t0_entry,
                .stack_ptr = &p3t3_stack[P3_TASK_STACK_SIZE - 1],
                .stack_size = P3_TASK_STACK_SIZE,
                .arg = p3t3_arg,
                .priority = P3_TASK_3_PRIO
        },
        {
                .entry = p3t0_entry,
                .stack_ptr = &p3t4_stack[P3_TASK_STACK_SIZE - 1],
                .stack_size = P3_TASK_STACK_SIZE,
                .arg = p3t4_arg,
                .priority = P3_TASK_4_PRIO
        }
};

partition_conf_t p3_conf = {
        .memory_conf = {
                .address = 0x95600000,
                .size = 0x100000,
        },
        .task_num = 5,
        .task_conf_list = p3_tasks,
        .slice_ticks = 5, // 5 partition timer intervals
        .target_core = 1
};
