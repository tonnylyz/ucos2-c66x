#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(p1_task_entry, ".text:PART_1")
#pragma CODE_SECTION(p1_apex_ipc_test, ".text:PART_1")
#pragma DATA_SECTION(p1_stack, ".data:PART_1")
#pragma DATA_SECTION(p1t1_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t2_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t3_arg, ".data:PART_1")
#pragma DATA_SECTION(p1t4_arg, ".data:PART_1")

#define P1_STACK_SIZE 16384
u8 p1_stack[P1_STACK_SIZE];

char p1t1_arg[] = "p1t1";
char p1t2_arg[] = "p1t2";
char p1t3_arg[] = "p1t3";
char p1t4_arg[] = "p1t4";

void p1_task_entry(void *arg) {
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

void p1_apex_ipc_test(void *arg) {
    char buf[20];
    return_code_t r;
    sampling_port_id_t pid;
    puts("p1_apex_ipc_test started\n");
    u_apex_get_sampling_port_id("port_2", &pid, &r);
    if (r != r_no_error) {
        puts("p1_apex_ipc_test u_apex_get_sampling_port_id failed\n");
        time_delay(0xffffff);
    }
    u32 actually_length;
    while (1) {
        validity_t validity;
        u_apex_read_sampling_port(pid, (message_addr_t) buf, &actually_length, &validity, &r);
        if (r == r_no_error) {
            break;
        } else {
            puts("p1_apex_ipc_test u_apex_read_sampling_port failed, retry\n");
            int i = 0xffff;
            while (i--) {
                asm(" NOP");
            }
        }
    }
    puts("p1_apex_ipc_test received length\n");
    putx(actually_length);
    putc('\n');
    puts(buf);
    putc('\n');
    while (1) {
        asm(" NOP");
    }
}

#define P1_TASK_NUM 4
#pragma DATA_SECTION(p1_task_conf_list, ".data:PART_S")
process_attribute_t p1_task_conf_list[P1_TASK_NUM] = {
        {
                .name = "p1t1",
                .entry_point = (u32) p1_apex_ipc_test,
                .stack_size = 2048,
                .arg = p1t1_arg,
                .base_priority = 11
        },
        {
                .name = "p1t2",
                .entry_point = (u32) p1_task_entry,
                .stack_size = 2048,
                .arg = p1t2_arg,
                .base_priority = 12
        },
        {
                .name = "p1t3",
                .entry_point = (u32) p1_task_entry,
                .stack_size = 2048,
                .arg = p1t3_arg,
                .base_priority = 13
        },
        {
                .name = "p1t4",
                .entry_point = (u32) p1_task_entry,
                .stack_size = 2048,
                .arg = p1t4_arg,
                .base_priority = 14
        }
};

#pragma DATA_SECTION(p1_port_conf_list, ".data:PART_S")
port_conf_t p1_port_conf_list[1] = {
        {
                .name = "port_2",
                .max_message_size = 1024,
                .direction = pd_destination,
                .refresh_period = 10,
                .peer_port = "port_1",
        },
};

#pragma DATA_SECTION(p1_conf, ".data:PART_S")
partition_conf_t p1_conf = {
        .identifier = 1,
        .memory_addr = 0x95400000,
        .memory_size = 0x100000,
        .stack_addr = (u32) p1_stack,
        .stack_size = P1_STACK_SIZE,
        .task_num = P1_TASK_NUM,
        .task_conf_list = p1_task_conf_list,
        .slice_num = 5,
        .target_core = 1,
        .port_num = 1,
        .port_conf_list = p1_port_conf_list,
};
