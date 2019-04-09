#include <partition_conf.h>
#include <syscall.h>

#pragma CODE_SECTION(root_task, ".text:PART_S")

void root_task(void *arg) {
    return_code_t r;
    partition_conf_t *conf;
    conf = arg;
    puts("[ROOT_TASK] Starting partition ");
    putx(conf->identifier);
    putc('\n');
    u8 i;
    for (i = 0; i < conf->task_num; i++) {
        u16 pid;
        u_apex_create_process(
            &(conf->task_conf_list[i]),
            &pid,
            &r
        );
        if (r != r_no_error) {
            puts("[ROOT_TASK] Adding partition failed\n");
        }
    }
    puts("[ROOT_TASK] Adding ports\n");
    for (i = 0; i < conf->port_num; i++) {
        sampling_port_id_t pid;
        u_apex_create_sampling_port(
                conf->port_conf_list[i].name,
                conf->port_conf_list[i].max_message_size,
                conf->port_conf_list[i].direction,
                conf->port_conf_list[i].refresh_period,
                &pid,
                &r
                );
        if (r != r_no_error) {
            puts("[ROOT_TASK] Adding port failed\n");
        }
    }

    puts("[ROOT_TASK] Ready to yield\n");
    u_apex_set_partition_mode(opm_normal, &r);
    task_change_priority(OS_PRIO_SELF, OS_TASK_IDLE_PRIO);
    while (1) {
        asm(" NOP");
    }
}
