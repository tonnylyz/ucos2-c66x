#include "task_conf.h"

#include <syscall.h>
#include <types.h>
#include <os_cfg.h>
#include <partition_conf.h>
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
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

void p0t1_entry(void *arg) {
    u8 buf[] = "hELLO wORLD!";
    while (task_ipc_send_foreign(2, 16, 6, (u32) buf, 13) != 0) {
        puts("p0t1_entry Retry\n");
        time_delay(1);
    }
    puts("p0t1_entry send Ok!\n");
    while (1) {
        puts("Name: p0t1\n");
        time_delay(5);
    }
}

/*  Inter-partition Comm Same Core

void p0t1_entry(void *arg) {
    u8 buf[20];
    u32 r;
    r = task_ipc_receive_foreign(1, (u32) buf, 20);
    puts("Task 1 received ");
    putc((char) ('0' + r));
    puts(": ");
    puts((char *) buf);
    putc('\n');

    while (1) {
        puts("Name: p0t1\n");
        time_delay(5);
    }
}*/

/*   Intra-partition Comm

void p0t1_entry(void *arg) {
    u32 r;
    puts("Task 1 is to receive\n");
    r = task_ipc_receive();
    puts("Task 1 received\n");
    putc((char) ('0' + r));
    putc('\n');
    while (1) {
        time_delay(5);
    }
}

void p0t2_entry(void *arg) {
    int r;
    r = task_ipc_send(P0_TASK_1_PRIO, 4);
    if (r < 0) {
        puts("Task 2 send failed\n");
    } else {
        puts("Task 2 send ok\n");
    }
    while (1) {
        time_delay(5);
    }
}
*/

void p0_idle_entry(void *arg) {
    while (1) {
        asm volatile ("\tNOP 5");
    }
}

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
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
                .entry = p0t1_entry,
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
        .identifier = 0,
        .memory_conf = {
                .address = 0x95300000,
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

        .task_num = 5,
        .task_conf_list = p0_tasks,
        .slice_ticks = 5, // 5 partition timer intervals
        .target_core = 0
};
