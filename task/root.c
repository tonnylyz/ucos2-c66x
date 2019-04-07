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
        u16 pid;
        u_apex_create_process(
            &(conf->task_conf_list[i]),
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
