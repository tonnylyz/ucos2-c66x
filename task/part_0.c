#include "part_0.h"
#include "syscall.h"
#include "part_1.h"
#include <partition_conf.h>
#include <printf.h>
#include <ucos_ii.h>

#pragma SET_CODE_SECTION(".text:PART_0")
#pragma SET_DATA_SECTION(".data:PART_0")

u8 p0t1_stack[P0_TASK_STACK_SIZE];
u8 p0t2_stack[P0_TASK_STACK_SIZE];
u8 p0t3_stack[P0_TASK_STACK_SIZE];
u8 p0t4_stack[P0_TASK_STACK_SIZE];
u8 p0_idle_stack[OS_TASK_IDLE_STK_SIZE];

char p0t1_arg[] = "p0t1";
char p0t2_arg[] = "p0t2";
char p0t3_arg[] = "p0t3";
char p0t4_arg[] = "p0t4";

void p0t0_entry(void *arg) {
    char *task_name = (char *) arg;
    //main_dummy = 123;
    while (1) {
        task_puts("Name: ");
        task_puts(task_name);
        task_putc('\n');
        task_time_delay(5);
    }
}

void p0_idle_entry(void *arg) {
    while (1) {
        asm volatile ("\tNOP 5");
    }
}

/* Partition 0 Configuration */

task_conf_t p0_tasks[5] = {
        {
                .entry = p0_idle_entry,
                .stack_ptr = &p0_idle_stack[OS_TASK_IDLE_STK_SIZE - 1],
                .stack_size = OS_TASK_IDLE_STK_SIZE,
                .arg = NULL,
                .priority = OS_TASK_IDLE_PRIO
        },
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
        }
};

partition_conf_t p0_conf = {
        .memory_conf = {
                .address = 0x95300000,
                .size = 0x100000,
        },
        .task_num = 5,
        .task_conf_list = p0_tasks,
        .slice_ticks = 5 // 5 partition timer intervals
};
