#include "apex.h"
#include "printf.h"
#include <partition.h>
#include <spinlock.h>

#ifdef APEX_POINTER_CHECK
static bool _is_malicious_pointer(const void *ptr, u32 size) {
    u32 hi, lo;
    lo = partition_current->conf->memory_addr;
    hi = lo + partition_current->conf->memory_size;
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
#endif

void apex_set_partition_mode(operating_mode_t ps, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
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
    *r = r_no_error;
}

void apex_get_partition_mode(operating_mode_t *pps, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(pps, sizeof(operating_mode_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    *pps = partition_current->operating_mode;
    *r = r_no_error;
}


void apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(name, APEX_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    int i;
    for (i = 0; i < partition_current->task_index; i++) {
        process_status_t *x = &partition_current->process_list[i];
        if (strcmp(x->attributes->name, name) == 0) {
            *ppid = x->pid;
            *r = r_no_error;
            return;
        }
    }
    *r = r_invalid_config;
}

void apex_get_process_status(process_id_t pid, process_status_t *pps, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(pps, sizeof(operating_mode_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    if (pid > partition_current->task_index) {
        *r = r_invalid_config;
        return;
    }
    *pps = partition_current->process_list[pid - 1];/* `pid` index from 1 */
    pps->tcb = NULL;
    *r = r_no_error;
}

void apex_create_process(process_attribute_t *ppa, process_id_t *ppid, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(ppa, sizeof(process_attribute_t))) {
        return;
    }
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    u32 i;
    for (i = 0; i < partition_current->task_index; i++) {
        if (strcmp(partition_current->process_list[i].attributes->name, ppa->name) == 0) {
            *r = r_no_action;
            return;
        }
    }
    if (ppa->base_priority == 0 || ppa->base_priority > OS_LOWEST_PRIO) {
        *r = r_invalid_param;
        return;
    }
    if (ppa->period != 0) {
        *r = r_invalid_param;
        return;
    }
    if (ppa->time_capacity != 0) {
        *r = r_invalid_param;
        return;
    }
    if (partition_current->operating_mode == opm_normal) {
        *r = r_invalid_mode;
        return;
    }
    partition_current->stack_ptr += ppa->stack_size;
    if (partition_current->stack_ptr - partition_current->conf->stack_addr > partition_current->conf->stack_size) {
        partition_current->stack_ptr -= ppa->stack_size;
        *r = r_invalid_config;
        return;
    }
    u8 e = OSTaskCreate(
            (void (*)(void *)) ppa->entry_point,
            ppa->arg,
            (OS_STK *) partition_current->stack_ptr,
            ppa->base_priority
            );
    if (e != OS_ERR_NONE) {
        *r = r_invalid_config;
        return;
    }
    i = partition_current->task_index;
    partition_current->process_list[i].attributes = ppa;
    partition_current->process_list[i].current_priority = ppa->base_priority;
    partition_current->process_list[i].process_state = ps_ready;
    partition_current->process_list[i].pid = i + 1;
    partition_current->process_list[i].tcb = OSTCBPrioTbl[ppa->base_priority];
    partition_current->process_list[i].tcb->OSTCBId = i + 1;
    partition_current->task_index ++;
    *ppid = i;
    *r = r_no_error;
}

void apex_set_priority(process_id_t pid, u8 priority, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
    if (pid - 1 >= partition_current->task_num) {
        *r = r_invalid_param;
        return;
    }
#endif
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
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    if (time_out == 0) {
        *r = r_no_error;
        return;
    }
    *r = r_no_error;
    OSTimeDly(time_out);
}

void apex_stop_self(void) {
    OSTaskDel(OS_PRIO_SELF);
}

void apex_get_my_id(process_id_t *ppid, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(ppid, sizeof(process_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    *ppid = OSTCBCur->OSTCBId;
    *r = r_no_error;
}



void apex_create_sampling_port(sampling_port_name_t sampling_port_name, message_size_t max_message_size,
                               port_direction_t port_direction, system_time_t refresh_period,
                               sampling_port_id_t *sampling_port_id, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(sampling_port_name, APEX_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(sampling_port_id, sizeof(sampling_port_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    sampling_port_t *port;
    port_conf_t *conf;
    conf = port_conf(partition_current->conf->port_conf_list, partition_current->conf->port_num, sampling_port_name);
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
    port->conf = conf;
    port->owner_partition = partition_current->identifier;
    *r = r_no_error;
}

static void _memcpy(void *dst, void *src, u32 len) {
    u32 i;
    for (i = 0; i < len; i++) {
        ((u8 *)dst)[i] = ((u8 *)src)[i];
    }
}

#define PORT_SPINLOCK_BASE 193

static inline void _port_lock(u8 id) {
    spinlock_lock(PORT_SPINLOCK_BASE + id);
}

static inline void _port_unlock(u8 id) {
    spinlock_unlock(PORT_SPINLOCK_BASE + id);
}

void apex_write_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t length,
                              return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer((const void *) message_addr, length)) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
    sampling_port_t *port;
    port = port_get(sampling_port_id);
    if (port == NULL) {
        *r = r_invalid_param;
        return;
    }
    _port_lock(sampling_port_id);
    if (port->status.port_direction != pd_source) {
        *r = r_invalid_mode;
        _port_unlock(sampling_port_id);
        return;
    }
    sampling_port_name_t peer = port->conf->peer_port;
    _port_unlock(sampling_port_id);
    if (!port_exist(peer)) {
        *r = r_invalid_config;
        return;
    }
    sampling_port_id_t peer_id = port_name2id(peer);
    _port_lock(peer_id);
    sampling_port_t *peer_port = port_get(peer_id);
    if (length > peer_port->conf->max_message_size) {
        *r = r_invalid_config;
        _port_unlock(peer_id);
        return;
    }
    peer_port->is_empty = false;
    peer_port->last_receive = OSTimeGet();
    _memcpy(peer_port->payload, (void *) message_addr, length);
    peer_port->actual_length = length;
    *r = r_no_error;
    _port_unlock(peer_id);
}

void apex_read_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t *length,
                             validity_t *validity, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
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
#endif
    sampling_port_t *port = port_get(sampling_port_id);
    if (port == NULL) {
        *r = r_invalid_param;
        return;
    }
    _port_lock(sampling_port_id);
    if (port->status.port_direction != pd_destination) {
        *r = r_invalid_mode;
        _port_unlock(sampling_port_id);
        return;
    }
    if (port->is_empty) {
        *validity = v_invalid;
        *r = r_no_action;
        _port_unlock(sampling_port_id);
        return;
    }
    _memcpy((void *) message_addr, port->payload, port->actual_length);
    *length = port->actual_length;
    *validity = v_valid;
    *r = r_no_error;
    _port_unlock(sampling_port_id);
}

void apex_get_sampling_port_id(sampling_port_name_t sampling_port_name, sampling_port_id_t *pid, return_code_t *r) {
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(sampling_port_name, APEX_NAME_MAX_LEN)) {
        return;
    }
    if (_is_malicious_pointer(pid, sizeof(sampling_port_id_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
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
#ifdef APEX_POINTER_CHECK
    if (_is_malicious_pointer(pstatus, sizeof(sampling_port_status_t))) {
        return;
    }
    if (_is_malicious_pointer(r, sizeof(return_code_t))) {
        return;
    }
#endif
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




