//
// Created by Tonny on 3/18/2019.
//

#ifndef UCOS2_C66X_APEX_H
#define UCOS2_C66X_APEX_H

#include <def.h>
#include <types.h>
#include "port.h"

void apex_set_partition_mode(operating_mode_t ps, return_code_t *r);
void apex_get_partition_mode(operating_mode_t *pps, return_code_t *r);

void apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r);
void apex_get_process_status(process_id_t pid, process_status_t *pps, return_code_t *r);
void apex_create_process(const process_attribute_t *ppa, process_id_t *ppid, return_code_t *r);
void apex_set_priority(process_id_t pid, u8 priority, return_code_t *r);
void apex_suspend_self(system_time_t time_out, return_code_t *r);
void apex_stop_self(void);
void apex_get_my_id(process_id_t *ppid, return_code_t *r);

void apex_create_sampling_port(sampling_port_name_t sampling_port_name, message_size_t max_message_size,
                               port_direction_t port_direction, system_time_t refresh_period,
                               sampling_port_id_t *sampling_port_id, return_code_t *r);
void apex_write_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t length,
                              return_code_t *r);
void apex_read_sampling_port(sampling_port_id_t sampling_port_id, message_addr_t message_addr, message_size_t *length,
                             validity_t *validity, return_code_t *r);
void apex_get_sampling_port_id(sampling_port_name_t sampling_port_name, sampling_port_id_t *pid, return_code_t *r);
void apex_get_sampling_port_status(sampling_port_id_t sampling_port_id, sampling_port_status_t *pstatus, return_code_t *r);

#endif //UCOS2_C66X_APEX_H
