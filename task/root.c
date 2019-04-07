#include <partition_conf.h>
#include <syscall.h>

#pragma CODE_SECTION(root_task, ".text:PART_S")

void root_task(void *arg) {
    return_code_t r;
    partition_conf_t *conf;
    conf = arg;
    puts("root_task started\n");
    puts("starting partition ");
    putc('0' + conf->identifier);
    putc('\n');
    u8 i;
    for (i = 0; i < conf->task_num; i++) {
        puts("adding ");
        puts(conf->task_conf_list[i].name);
        putc('\n');
        process_attribute_t attribute = (process_attribute_t) {
                .name = conf->task_conf_list[i].name,
                .deadline = ddl_soft,
                .base_priority = conf->task_conf_list[i].priority,
                .entry_point = (u32) conf->task_conf_list[i].entry,
                .period = 0,
                .stack_size = conf->task_conf_list[i].stack_size,
                .time_capacity = 0,
                .arg = conf->task_conf_list[i].arg,
        };
        u16 pid;
        u_apex_create_process(
            &attribute,
            &pid,
            &r
        );
    }
    u_apex_set_partition_mode(opm_normal, &r);
    task_change_priority(OS_PRIO_SELF, OS_TASK_IDLE_PRIO);
    puts("act as idle task\n");
    while (1) {
        asm(" NOP");
    }
}
