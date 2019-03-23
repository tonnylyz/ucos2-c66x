//
// Created by Tonny on 3/18/2019.
//

#ifndef UCOS2_C66X_DEF_H
#define UCOS2_C66X_DEF_H

#include <types.h>

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

#define PORT_MAX_NUMBER 128

typedef u32 message_addr_t;
typedef u32 message_size_t;

typedef enum {
    pd_source,
    pd_destination,
} port_direction_t;

typedef u32 sampling_port_id_t;
typedef char* sampling_port_name_t;
typedef enum {
    v_invalid,
    v_valid,
} validity_t;

typedef struct {
    u32 max_message_size;
    port_direction_t port_direction;
    system_time_t refresh_period;
    validity_t last_msg_validity;
} sampling_port_status_t;

typedef struct {
    sampling_port_id_t id;
    sampling_port_status_t status;
} sampling_port_t;

typedef u32 queuing_port_id_t;
typedef char* queuing_port_name_t;

typedef enum {
    qd_fifo,
    qd_priority,
} queuing_discipline_t;

typedef u32 message_range_t;
typedef u32 waiting_range_t;

typedef struct {
    message_range_t nb_message;
    message_range_t max_nb_message;
    message_size_t max_message_size;
    port_direction_t port_direction;
    waiting_range_t waiting_processes;
} queuing_port_status_t;



return_code_t apex_create_sampling_port(
        sampling_port_name_t sampling_port_name,
        message_size_t max_message_size,
        port_direction_t port_direction,
        system_time_t refresh_period,
        sampling_port_id_t *p_sampling_port_id);


#endif //UCOS2_C66X_DEF_H
