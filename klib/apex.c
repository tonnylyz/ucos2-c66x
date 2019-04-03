//
// Created by Tonny on 3/18/2019.
//

#include "apex.h"
#include "printf.h"
#include <partition.h>

/* syscall 20 */
void apex_set_partition_mode(operating_mode_t ps, return_code_t *r) {
    if (ps != opm_cold_start && ps != opm_idle && ps != opm_normal && ps != opm_warm_start) {
        *r = r_invalid_param;
    }
    if (ps == opm_normal && partition_current->operating_mode == opm_normal) {
        *r = r_no_action;
    }
    if (ps == opm_warm_start && partition_current->operating_mode == opm_cold_start) {
        *r = r_invalid_mode;
    }
    partition_current->operating_mode = ps;
    if (ps == opm_idle) {
        // shutdown the partition
        partition_current->target_core = 255u;
    } else {
        printf("Warning: setting partition_mode except for idle mode deal no change to the system");
    }
    *r = r_no_error;
}

/* syscall 21 */
void apex_get_partition_mode(operating_mode_t *pps, return_code_t *r) {
    *pps = partition_current->operating_mode;
    *r = r_no_error;
}

u32 _strlen(const char *str) {
    u32 i;
    for (i = 0; str[i] == '\0'; i++);
    return i;
}

bool _str_equal(const char *a, const char *b) {
    u32 i;
    u32 a_len, b_len;
    a_len = _strlen(a);
    b_len = _strlen(b);
    if (a_len != b_len) return false;
    for (i = 0; i < a_len; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r) {
    int i;
    for (i = 0; i < partition_current->task_num; i++) {
        process_status_t *x = &partition_current->process_list[i];
        if (_str_equal(x->attributes.name, name)) {
            *ppid = x->pid;
            *r = r_no_error;
            return;
        }
    }
    *r = r_invalid_config;
}

void apex_get_process_status(process_id_t pid, process_status_t *pps, return_code_t *r) {
    if (pid > partition_current->task_num) {
        *r = r_invalid_config;
        return;
    }
    *pps = partition_current->process_list[pid - 1];
    *r = r_no_error;
}


