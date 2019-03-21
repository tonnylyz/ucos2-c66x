#include "part_1.h"

#include <syscall.h>
#include <types.h>
#include <os_cfg.h>
#include <partition_conf.h>
#include <ucos_ii.h>

#pragma SET_CODE_SECTION(".text:PART_1")
#pragma SET_DATA_SECTION(".data:PART_1")

u8 p1t1_stack[P1_TASK_STACK_SIZE];
u8 p1t2_stack[P1_TASK_STACK_SIZE];
u8 p1t3_stack[P1_TASK_STACK_SIZE];
u8 p1_idle_stack[OS_TASK_IDLE_STK_SIZE];

char p1t1_arg[] = "p1t1";
char p1t2_arg[] = "p1t2";
char p1t3_arg[] = "p1t3";

void make_xmc_error() {
    extern u8 p0t1_stack[];
    p0t1_stack[1] = 0;
}

void p1t0_entry(void *arg) {
    //make_xmc_error();
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(4);
    }
}

void p1_idle_entry(void *arg) {
    while (1) {
        asm volatile ("\tNOP 5");
    }
}

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
/* Partition 1 Configuration */

task_conf_t p1_tasks[5] = {
        {
                .entry = p1_idle_entry,
                .stack_ptr = &p1_idle_stack[OS_TASK_IDLE_STK_SIZE - 1],
                .stack_size = OS_TASK_IDLE_STK_SIZE,
                .arg = NULL,
                .priority = OS_TASK_IDLE_PRIO
        },
        {
                .entry = p1t0_entry,
                .stack_ptr = &p1t1_stack[P1_TASK_STACK_SIZE - 1],
                .stack_size = P1_TASK_STACK_SIZE,
                .arg = p1t1_arg,
                .priority = P1_TASK_1_PRIO
        },
        {
                .entry = p1t0_entry,
                .stack_ptr = &p1t2_stack[P1_TASK_STACK_SIZE - 1],
                .stack_size = P1_TASK_STACK_SIZE,
                .arg = p1t2_arg,
                .priority = P1_TASK_2_PRIO
        },
        {
                .entry = p1t0_entry,
                .stack_ptr = &p1t3_stack[P1_TASK_STACK_SIZE - 1],
                .stack_size = P1_TASK_STACK_SIZE,
                .arg = p1t3_arg,
                .priority = P1_TASK_3_PRIO
        }
};

partition_conf_t p1_conf = {
        .memory_conf = {
                .address = 0x95400000,
                .size = 0x100000,
        },
        .task_num = 4,
        .task_conf_list = p1_tasks,
        .slice_ticks = 10, // 10 partition timer intervals
        .target_core = 0
};
