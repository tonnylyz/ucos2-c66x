//
// Created by Tonny on 3/18/2019.
//

#include "apex.h"
#include "printf.h"
#include <partition.h>

static bool _is_malicious_pointer(const void *ptr, u32 size) {
    u32 hi, lo;
    lo = partition_current->conf->memory_conf.address;
    hi = lo + partition_current->conf->memory_conf.size;
    if ((u32)ptr < lo) {
        return true;
    }
    if ((u32)ptr >= hi) {
        return true;
    }
    if ((u32)ptr + size > hi) {
        return true;
    }
    return false;
}

void apex_set_partition_mode(operating_mode_t ps, return_code_t *r) {
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
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

void apex_get_partition_mode(operating_mode_t *pps, return_code_t *r) {
    if (_is_malicious_pointer(pps, sizeof(operating_mode_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    *pps = partition_current->operating_mode;
    *r = r_no_error;
}

static u32 _strlen(const char *str) {
    u32 i;
    for (i = 0; str[i] == '\0' && i < PROCESS_ATTR_NAME_MAX_LEN; i++);
    return i;
}

static bool _str_equal(const char *a, const char *b) {
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
    if (_is_malicious_pointer(name, PROCESS_ATTR_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
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
    if (_is_malicious_pointer(pps, sizeof(operating_mode_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    if (pid > partition_current->task_num) {
        *r = r_invalid_config;
        return;
    }
    *pps = partition_current->process_list[pid - 1];/* `pid` index from 1 */
    pps->tcb = NULL;
    *r = r_no_error;
}

void apex_create_process(const process_attribute_t *ppa, process_id_t *ppid, return_code_t *r) {
    if (_is_malicious_pointer(ppa, sizeof(process_attribute_t))) {
        return;
    }
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    /* WARNING:
     *  This function is not used at runtime. You must NOT invoke this syscall at runtime.
     *  This function is designed to provide an alternative to initialize a task in partition.
     *  If you want to use this APEX service, you need to remove the task adding procedure in `partition_add`,
     *  also this function is not ready for this use.
     * */
    u32 i;
    for (i = 0; i < partition_current->task_num; i++) {
        if (_str_equal(partition_current->process_list[i].attributes.name, ppa->name)) {
            *r = r_no_action;
            return;
        }
    }
    if (ppa->stack_size > partition_current->conf->memory_conf.size /* A CONFIGURATION TIME STACK SIZE LIMIT */) {
        *r = r_invalid_param;
        return;
    }
    if (ppa->base_priority == 0 || ppa->base_priority > OS_LOWEST_PRIO) {
        *r = r_invalid_param;
        return;
    }
    if (ppa->period > 0xffff /* A CONFIGURATION TIME PERIOD LIMIT */) {
        /* DUMMY */
        *r = r_invalid_param;
        return;
    }
    if (ppa->time_capacity > 0xffff /* A CONFIGURATION TIME CAPACITY LIMIT */) {
        /* DUMMY */
        *r = r_invalid_param;
        return;
    }
    if (partition_current->operating_mode == opm_normal) {
        *r = r_invalid_mode;
        return;
    }
    u8 e = OSTaskCreate(
            (void (*)(void *)) ppa->entry_point,
            NULL/* ARG */,
            NULL/* STK PTR (NEED ALLOCATED BY KERNEL) */,
            ppa->base_priority
            );
    if (e != OS_ERR_NONE) {
        *r = r_invalid_config;
        return;
    }
    i = partition_current->task_num;
    partition_current->process_list[i].attributes = *ppa;
    partition_current->process_list[i].current_priority = ppa->base_priority;
    partition_current->process_list[i].deadline_time = 0;
    partition_current->process_list[i].process_state = ps_ready;
    partition_current->process_list[i].pid = i + 1;
    partition_current->process_list[i].tcb = partition_current->context.OSTCBPrioTbl[ppa->base_priority];
    partition_current->task_num ++;
    *ppid = i;
    *r = r_no_error;
}

void apex_set_priority(process_id_t pid, u8 priority, return_code_t *r) {
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    if (pid - 1 >= partition_current->task_num) {
        *r = r_invalid_param;
        return;
    }
    if (priority == 0 || priority > OS_LOWEST_PRIO) {
        *r = r_invalid_param;
        return;
    }
    if (partition_current->process_list[pid - 1].process_state == ps_dormant) {
        *r = r_invalid_mode;
        return;
    }
    if (partition_current->context.OSTCBPrioTbl[priority] != NULL) {
        *r = r_invalid_param;
        return;
    }
    OSTaskChangePrio(partition_current->process_list[pid- 1].tcb->OSTCBPrio, priority);
}
