#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p0_task_entry, ".text:PART_0")
#pragma CODE_SECTION(p0_apex_ipc_test, ".text:PART_0")
#pragma DATA_SECTION(p0_stack, ".data:PART_0")
#pragma DATA_SECTION(p0t1_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t2_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t3_arg, ".data:PART_0")
#pragma DATA_SECTION(p0t4_arg, ".data:PART_0")

#define P1_STACK_SIZE 16384
u8 p0_stack[P1_STACK_SIZE];

char p0t1_arg[] = "p0t1";
char p0t2_arg[] = "p0t2";
char p0t3_arg[] = "p0t3";
char p0t4_arg[] = "p0t4";

void p0_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

void p0_apex_ipc_test(void *arg) {
    char buf[] = "Hello World!";
    return_code_t r;
    sampling_port_id_t pid;
    puts("p0_apex_ipc_test started\n");
    while (1) {
        u_apex_get_sampling_port_id("port_2", &pid, &r);
        if (r != r_no_error) {
            puts("p0_apex_ipc_test u_apex_get_sampling_port_id port_2 failed retry\n");
        } else {
            break;
        }
    }
    while (1) {
        u_apex_get_sampling_port_id("port_1", &pid, &r);
        if (r != r_no_error) {
            puts("p0_apex_ipc_test u_apex_get_sampling_port_id port_1 failed retry\n");
        } else {
            break;
        }
    }
    u_apex_write_sampling_port(pid, (message_addr_t) buf, 13, &r);
    if (r != r_no_error) {
        puts("p0_apex_ipc_test u_apex_write_sampling_port failed\n");
        time_delay(0xffffff);
    }
    while (1) {
        asm(" NOP");
    }
}

#define P0_TASK_NUM 4
#pragma DATA_SECTION(p0_task_conf_list, ".data:PART_S")
process_attribute_t p0_task_conf_list[P0_TASK_NUM] = {
        {
                .name = "p0t1",
                .entry_point = (u32) p0_apex_ipc_test,
                .stack_size = 2048,
                .arg = p0t1_arg,
                .base_priority = 11
        },
        {
                .name = "p0t2",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t2_arg,
                .base_priority = 12
        },
        {
                .name = "p0t3",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t3_arg,
                .base_priority = 13
        },
        {
                .name = "p0t4",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p0_port_conf_list, ".data:PART_S")
port_conf_t p0_port_conf_list[1] = {
        {
                .name = "port_1",
                .max_message_size = 1024,
                .direction = pd_source,
                .refresh_period = 10,
                .peer_port = "port_2",
        },
};

#pragma DATA_SECTION(p0_conf, ".data:PART_S")
partition_conf_t p0_conf = {
        .identifier = 0,
        .memory_addr = 0x95300000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p0_stack,
        .stack_size = P1_STACK_SIZE,
        .task_num = P0_TASK_NUM,
        .task_conf_list = p0_task_conf_list,
        .slice_num = 5,
        .target_core = 0,
        .port_num = 1,
        .port_conf_list = p0_port_conf_list,
};
