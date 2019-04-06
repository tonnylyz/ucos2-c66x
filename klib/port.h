//
// Created by Tonny on 4/6/2019.
//

#ifndef UCOS2_C66X_PORT_H
#define UCOS2_C66X_PORT_H

#include "def.h"
#define PORT_MAX_NUM 8

typedef u32 message_addr_t;
typedef u32 message_size_t;

typedef enum {
    pd_source,
    pd_destination,
} port_direction_t;

typedef u32 sampling_port_id_t;
typedef const char* sampling_port_name_t;
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
    char name[APEX_NAME_MAX_LEN];
    u32 max_message_size;
    port_direction_t direction;
    system_time_t refresh_period;
} port_conf_t;

typedef struct {
    sampling_port_id_t id;
    sampling_port_status_t status;
    port_conf_t *conf;
    u8 payload[IPC_INTER_PARTITION_MAX_LENGTH];
    bool is_empty;
    u32 actual_length;
} sampling_port_t;



extern sampling_port_t ports[PORT_MAX_NUM];
extern u8 ports_index;

extern port_conf_t port_conf_list[];
extern u8 port_conf_num;

sampling_port_t *port_alloc();

#endif //UCOS2_C66X_PORT_H
