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
typedef char * sampling_port_name_t;
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
    char peer_port[APEX_NAME_MAX_LEN];

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

    u8 owner_partition;
    system_time_t last_receive;
} sampling_port_t;

sampling_port_t *port_alloc(void);

sampling_port_t *port_get(u8 id);

bool port_exist(char *name);

u8 port_name2id(char *name);

static inline port_conf_t *port_conf(port_conf_t *port_conf_list, u8 port_conf_num, char *name) {
    u8 i;
    for (i = 0; i < port_conf_num; i++) {
        if (strcmp(name, port_conf_list[i].name) == 0) {
            return &(port_conf_list[i]);
        }
    }
    return NULL;
}
#endif //UCOS2_C66X_PORT_H