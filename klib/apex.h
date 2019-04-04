//
// Created by Tonny on 3/18/2019.
//

#ifndef UCOS2_C66X_APEX_H
#define UCOS2_C66X_APEX_H

#include <def.h>
#include <types.h>

void apex_set_partition_mode(operating_mode_t ps, return_code_t *r);
void apex_get_partition_mode(operating_mode_t *pps, return_code_t *r);

void apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r);
void apex_get_process_status(process_id_t pid, process_status_t *pps, return_code_t *r);
void apex_create_process(const process_attribute_t *ppa, process_id_t *ppid, return_code_t *r);
void apex_set_priority(process_id_t pid, u8 priority, return_code_t *r);
void apex_suspend_self(system_time_t time_out, return_code_t *r);
void apex_stop_self(void);
void apex_get_my_id(process_id_t *ppid, return_code_t *r);

void apex_create_sampling_port();
void apex_write_sampling_port();
void apex_read_sampling_port();
void apex_get_sampling_port_id();
void apex_get_sampling_port_status();

#endif //UCOS2_C66X_APEX_H
