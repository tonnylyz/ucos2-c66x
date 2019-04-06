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


void apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r) {
    if (_is_malicious_pointer(name, APEX_NAME_MAX_LEN)) {
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

void apex_suspend_self(system_time_t time_out, return_code_t *r) {
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    /* preemption is always enabled
     * there is no error handler process
     * */
    if (time_out > 0xffff) {
        *r = r_invalid_param;
        return;
    }
    u16 pid = OSTCBCur->OSTCBId;
    if (partition_current->process_list[pid - 1].attributes.period != 0) {
        *r = r_invalid_mode;
        return;
    }

    if (time_out == 0) {
        *r = r_no_error;
        return;
    }

    partition_current->process_list[pid - 1].process_state = ps_waiting;
    *r = r_no_error;
    if (time_out != 0xffff) {
        OSTimeDly(time_out);
    } /* No support for finite time out */
    /* expiration of time out logic not presented */
}

void apex_stop_self(void) {
    OSTaskDel(OS_PRIO_SELF);
}

void apex_get_my_id(process_id_t *ppid, return_code_t *r) {
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    *ppid = OSTCBCur->OSTCBId;
    *r = r_no_error;
}

port_conf_t *_port_conf(const char *name) {
    u8 i;
    for (i = 0; i < PORT_CONF_NUM; i++) {
        if (_str_equal(name, port_conf_list[i].name)) {
            return &(port_conf_list[i]);
        }
    }
    return NULL;
}

void apex_create_sampling_port(sampling_port_name_t sampling_port_name, message_size_t max_message_size,
                               port_direction_t port_direction, system_time_t refresh_period,
                               sampling_port_id_t *sampling_port_id, return_code_t *r) {


    if (_is_malicious_pointer(sampling_port_name, APEX_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(sampling_port_id, sizeof(sampling_port_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }

    sampling_port_t *port;
    port_conf_t *conf;
    conf = _port_conf(sampling_port_name);
    if (conf == NULL) {
        *r = r_invalid_config;
        return;
    }
    if (port_exist(sampling_port_name)) {
        *r = r_no_action;
        return;
    }
    if (max_message_size > IPC_INTER_PARTITION_MAX_LENGTH || max_message_size != conf->max_message_size) {
        *r = r_invalid_config;
        return;
    }
    if (port_direction != conf->direction) {
        *r = r_invalid_config;
        return;
    }
    if (refresh_period != conf->refresh_period) {
        *r = r_invalid_config;
        return;
    }
    if (partition_current->operating_mode == opm_normal) {
        /* This function does NOT serve as a syscall currently.
         * This function is invoked by kernel to initialize ports at present.
         * */
        *r = r_invalid_mode;
        return;
    }
    port = port_alloc();
    if (port == NULL) {
        *r = r_invalid_config;
        return;
    }
    *sampling_port_id = port->id;
    port->status.port_direction = port_direction;
    port->status.max_message_size = max_message_size;
    port->status.refresh_period = refresh_period;
    *r = r_no_error;
}

void _memcpy(void *dst, void *src, u32 len) {
    u32 i;
    for (i = 0; i < len; i++) {
        ((u8 *)dst)[i] = ((u8 *)src)[i];
    }
}

void apex_write_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t length,
                              return_code_t *r) {


    if (_is_malicious_pointer((const void *) message_addr, length)) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }

    sampling_port_t *port = port_get(sampling_port_id);
    if (port == NULL) {
        *r = r_invalid_param;
        return;
    }
    if (length > port->conf->max_message_size) {
        *r = r_invalid_config;
        return;
    }
    if (port->status.port_direction != pd_source) {
        *r = r_invalid_mode;
    }
    _memcpy(port->payload, (void *) message_addr, length);
    *r = r_no_error;
}

void apex_read_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t *length,
                             validity_t *validity, return_code_t *r) {


    if (_is_malicious_pointer((const void *) message_addr, IPC_INTER_PARTITION_MAX_LENGTH)) {
        return;
    }
    if (_is_malicious_pointer(length, sizeof(message_size_t))) {
        return;
    }
    if (_is_malicious_pointer(validity, sizeof(validity_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }


    sampling_port_t *port = port_get(sampling_port_id);
    if (port == NULL) {
        *r = r_invalid_param;
        return;
    }
    if (port->status.port_direction != pd_destination) {
        *r = r_invalid_mode;
        return;
    }
    if (port->is_empty) {
        *validity = v_invalid;
        *r = r_no_action;
        return;
    }
    _memcpy((void *) message_addr, port->payload, port->actual_length);
    *length = port->actual_length;
    if (true /* age of the copied message is consistent with the required REFRESH_PERIOD
attribute of the port */) {
        *validity = v_valid;
    } else {
        *validity = v_invalid;
    }
    *r = r_no_error;
    /* update validity in status of the port; */
}

void apex_get_sampling_port_id(sampling_port_name_t sampling_port_name, sampling_port_id_t *pid, return_code_t *r) {

    if (_is_malicious_pointer(sampling_port_name, APEX_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(pid, sizeof(sampling_port_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }

    u8 i;
    i = port_name2id(sampling_port_name);
    if (i == 0xff) {
        *r = r_invalid_config;
        return;
    }
    *r = r_no_error;
    *pid = i;
}

void apex_get_sampling_port_status(sampling_port_id_t sampling_port_id, sampling_port_status_t *pstatus, return_code_t *r) {

    if (_is_malicious_pointer(pstatus, sizeof(sampling_port_status_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }

    sampling_port_t *port = port_get(sampling_port_id);
    if (port == NULL) {
        *r = r_invalid_param;
        return;
    }
    *pstatus = port->status;
    /*  -- The status should provide the LAST_MSG_VALIDITY that is the validity of
        -- the last read message in the port.*/
    *r = r_no_error;
}




