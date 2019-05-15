#include <types.h>
#include <syscall.h>
#include <partition_conf.h>

#pragma CODE_SECTION(task_receiver, ".text:PART_0")
#pragma CODE_SECTION(task_sender, ".text:PART_0")
#pragma CODE_SECTION(task_receiver_foreign, ".text:PART_0")
#pragma CODE_SECTION(p0_task_entry, ".text:PART_0")
#pragma CODE_SECTION(task_sender_apex, ".text:PART_0")
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

void task_receiver(void *arg) {
    u32 r;
    puts("task_receiver start to run\n");
    r = task_ipc_receive();
    puts("task_receiver received ");
    putx(r);
    putc('\n');
}

void task_sender(void *arg) {
    int r;
    puts("task_sender start to run\n");
    r = task_ipc_send(11, 0x12345678);
    if (r == 0) {
        puts("task_sender sent succeeded\n");
    }
}

void task_receiver_foreign(void *arg) {
    u32 r;
    u8 buf[32];
    puts("task_receiver_foreign start to run\n");
    r = task_ipc_receive_foreign(1, buf, 32);
    puts("task_receiver_foreign received value: ");
    putx(r);
    putc('\n');
    puts("task_receiver_foreign received content: ");
    puts(buf);
    putc('\n');
    task_delete(OS_PRIO_SELF);
}



void task_sender_apex(void *arg) {
    char buf[] = "Hello World!";
    return_code_t r;
    sampling_port_id_t pid;
    puts("task_sender_apex start to run\n");
    while (1) {
        u_apex_get_sampling_port_id("port_2", &pid, &r);
        if (r == r_no_error) {
            puts("task_sender_apex get_sampling_port_id port_2 succeeded\n");
            break;
        }
    }
    u_apex_get_sampling_port_id("port_1", &pid, &r);
    if (r == r_no_error) {
        puts("task_sender_apex get_sampling_port_id port_1 succeeded\n");
    }
    u_apex_write_sampling_port(pid, (message_addr_t) buf, 13, &r);
    if (r == r_no_error) {
        puts("task_sender_apex write_sampling_port succeeded\n");
    }
    task_delete(OS_PRIO_SELF);
}


void p0_task_entry(void *arg) {
    task_delete(OS_PRIO_SELF);
    char *task_name = (char *) arg;
    while (1) {
        puts("Name: ");
        puts(task_name);
        putc('\n');
        time_delay(5);
    }
}

#define P0_TASK_NUM 1
#pragma DATA_SECTION(p0_task_conf_list, ".data:PART_S")
process_attribute_t p0_task_conf_list[P0_TASK_NUM] = {
        {
                .name = "p0t1",
                .entry_point = (u32) p0_task_entry,
                .stack_size = 2048,
                .arg = p0t1_arg,
                .base_priority = 11
        }/*,
        {
                .name = "p0t2",
                .entry_point = (u32) task_sender,
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
        }*/
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
        .slice_num = 1,
        .target_core = 0,
        .port_num = 1,
        .port_conf_list = p0_port_conf_list,
};
