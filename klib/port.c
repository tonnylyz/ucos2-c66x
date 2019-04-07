#include "port.h"

#pragma DATA_SECTION(port_conf_list, ".data:KERN_SHARE")
/************ [ Configuration Time Section ] ************/
port_conf_t port_conf_list[PORT_CONF_NUM] = {
        (port_conf_t) {
                .name = "PORT_1",
                .max_message_size = 1024,
                .direction = pd_source,
                .refresh_period = 10,
        },
        (port_conf_t) {
                .name = "PORT_2",
                .max_message_size = 1024,
                .direction = pd_destination,
                .refresh_period = 10,
        }
};
/************=[============================]=************/


#pragma DATA_SECTION(ports_index, ".data:KERN_SHARE")
static u8 ports_index = 0;
#pragma DATA_SECTION(ports, ".data:KERN_SHARE")
static sampling_port_t ports[PORT_MAX_NUM];

sampling_port_t *port_alloc(void) {
    if (ports_index >= PORT_MAX_NUM) {
        return NULL;
    }
    sampling_port_t *port = &(ports[ports_index]);
    port->id = ports_index;
    ports_index++;
    return port;
}

sampling_port_t *port_get(u8 id) {
    if (id >= ports_index) {
        return NULL;
    }
    return &(ports[id]);
}

bool port_exist(const char *name) {
    u8 i;
    for (i = 0; i < ports_index; i++) {
        if (strcmp(name, ports[i].conf->name) == 0) {
            return true;
        }
    }
    return false;
}

u8 port_name2id(const char *name) {
    u8 i;
    for (i = 0; i < ports_index; i++) {
        if (strcmp(name, ports[i].conf->name) == 0) {
            return i;
        }
    }
    return 255;
}
