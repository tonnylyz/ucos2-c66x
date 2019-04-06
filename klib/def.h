//
// Created by Tonny on 3/18/2019.
//

#ifndef UCOS2_C66X_DEF_H
#define UCOS2_C66X_DEF_H

#include <types.h>
#include <ucos_ii.h>

typedef enum {
    sc_normal_start,
    sc_partition_restart,
    sc_hm_module_restart,
    sc_hm_partition_restart,
} start_condition_t;

typedef enum {
    opm_idle,
    opm_normal,
    opm_cold_start,
    opm_warm_start,
} operating_mode_t;

typedef enum {
    r_no_error,
    r_no_action,
    r_not_available,
    r_invalid_param,
    r_invalid_config,
    r_invalid_mode,
    r_timed_out,
} return_code_t;

typedef enum {
    pt_system,
    pt_normal,
} partition_type_t;

typedef u32 system_time_t;

typedef u16 process_id_t;

typedef enum {
    ddl_soft,
    ddl_hard,
} deadline_t;

typedef enum {
    ps_dormant,
    ps_ready,
    ps_running,
    ps_waiting,
} process_state_t;

#define APEX_NAME_MAX_LEN 16

typedef struct {
    char name[APEX_NAME_MAX_LEN];
    u32 entry_point;
    u32 stack_size;
    u8  base_priority;
    system_time_t period;
    system_time_t time_capacity;
    deadline_t deadline;
} process_attribute_t;

typedef struct {
    process_attribute_t attributes;
    u8 current_priority;
    system_time_t deadline_time;
    process_state_t process_state;
    OS_TCB *tcb;
    u16 pid;
} process_status_t;

#endif //UCOS2_C66X_DEF_H
