//
// Created by Tonny on 4/6/2019.
//

#include "port.h"

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
/************ [ Configuration Time Section ] ************/
u8 port_conf_num = 2;
port_conf_t port_conf_list[2] = {
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


u8 ports_index = 0;
sampling_port_t ports[PORT_MAX_NUM];

sampling_port_t *port_alloc() {
    if (ports_index >= PORT_MAX_NUM) {
        return NULL;
    }
    sampling_port_t *port = &(ports[ports_index]);
    port->id = ports_index;
    ports_index++;
    return port;
}
