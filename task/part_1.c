#include "task_conf.h"

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

void make_xmc_error(void) {
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

/*  Inter-partition Comm Same Core

 void p1t1_entry(void *arg) {
    int r;
    u8 buf[] = "Hello World!";
    r = task_ipc_send_foreign(0, 11, 5, (u32) buf, 13);
    if (r < 0) {
        puts("task_ipc_send_foreign failed ");
        putc((char) ('0' - r));
        putc('\n');
    } else {
        puts("task_ipc_send_foreign Ok\n");
    }
    while (1) {
        puts("Name: p1t1\n");
        time_delay(4);
    }
}*/

void p1_idle_entry(void *arg) {
    while (1) {
        asm(" NOP");
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
        .identifier = 1,
        .memory_conf = {
                .address = 0x95400000,
                .size = 0x100000,
        },
        .period = 0,
        .duration = 0,
        .critical_level = 0,
        .communication_conf = {
                .dest_num = 0,
                .dest_list = NULL,
                .src_num = 0,
                .src_list = NULL,
        },
        .entry_point = 0,
        .type = pt_normal,

        .task_num = 4,
        .task_conf_list = p1_tasks,
        .slice_ticks = 10, // 10 partition timer intervals
        .target_core = 0
};
