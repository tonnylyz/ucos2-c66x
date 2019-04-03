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



#endif //UCOS2_C66X_APEX_H
