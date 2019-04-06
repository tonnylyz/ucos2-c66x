#include "task_conf.h"

#include <syscall.h>
#include <types.h>
#include <os_cfg.h>
#include <partition_conf.h>
#include <ucos_ii.h>

#pragma SET_CODE_SECTION(".text:PART_2")
#pragma SET_DATA_SECTION(".data:PART_2")

u8 p2t1_stack[P2_TASK_STACK_SIZE];
u8 p2t2_stack[P2_TASK_STACK_SIZE];
u8 p2t3_stack[P2_TASK_STACK_SIZE];
u8 p2_idle_stack[OS_TASK_IDLE_STK_SIZE];

char p2t1_arg[] = "p2t1";
char p2t2_arg[] = "p2t2";
char p2t3_arg[] = "p2t3";

void p2t0_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(4);
    }
}

void p2t1_entry(void *arg) {
    u32 r;
    u8 buf[128];
    r = task_ipc_receive_foreign(0, (u32) buf, 128);
    puts("p2t1_entry: task_ipc_receive_foreign ");
    putc((char) ('0' + r));
    putc(':');
    puts((char *) buf);
    putc('\n');
    while (1) {
        puts("Name: p2t1\n");
        time_delay(4);
    }
}

void p2_idle_entry(void *arg) {
    while (1) {
        asm(" NOP");
    }
}

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
/* Partition 2 Configuration */

task_conf_t p2_tasks[5] = {
        {
                .entry = p2_idle_entry,
                .stack_ptr = &p2_idle_stack[OS_TASK_IDLE_STK_SIZE - 1],
                .stack_size = OS_TASK_IDLE_STK_SIZE,
                .arg = NULL,
                .priority = OS_TASK_IDLE_PRIO
        },
        {
                .entry = p2t1_entry,
                .stack_ptr = &p2t1_stack[P2_TASK_STACK_SIZE - 1],
                .stack_size = P2_TASK_STACK_SIZE,
                .arg = p2t1_arg,
                .priority = P2_TASK_1_PRIO
        },
        {
                .entry = p2t0_entry,
                .stack_ptr = &p2t2_stack[P2_TASK_STACK_SIZE - 1],
                .stack_size = P2_TASK_STACK_SIZE,
                .arg = p2t2_arg,
                .priority = P2_TASK_2_PRIO
        },
        {
                .entry = p2t0_entry,
                .stack_ptr = &p2t3_stack[P2_TASK_STACK_SIZE - 1],
                .stack_size = P2_TASK_STACK_SIZE,
                .arg = p2t3_arg,
                .priority = P2_TASK_3_PRIO
        }
};

partition_conf_t p2_conf = {
        .identifier = 2,
        .memory_conf = {
                .address = 0x95500000,
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
        .task_conf_list = p2_tasks,
        .slice_ticks = 10, // 10 partition timer intervals
        .target_core = 1
};
