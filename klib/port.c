#include "port.h"
#include <printf.h>
#include <spinlock.h>

#pragma DATA_SECTION(ports_index, ".data:KERN_SHARE")
static u8 ports_index = 0;
#pragma DATA_SECTION(ports, ".data:KERN_SHARE")
static sampling_port_t ports[PORT_MAX_NUM];

#define PORT_LIST_SPINLOCK 192

sampling_port_t *port_alloc(void) {
    if (ports_index >= PORT_MAX_NUM) {
        return NULL;
    }
    spinlock_lock(PORT_LIST_SPINLOCK);
    sampling_port_t *port = &(ports[ports_index]);
    OS_MemClr((INT8U *) port, sizeof(sampling_port_t));
    port->id = ports_index;
    port->is_empty = true;
    ports_index++;
    spinlock_unlock(PORT_LIST_SPINLOCK);
    return port;
}

sampling_port_t *port_get(u8 id) {
    if (id >= ports_index) {
        return NULL;
    }
    return &(ports[id]);
}

bool port_exist(char *name) {
    u8 i;
    spinlock_lock(PORT_LIST_SPINLOCK);
    for (i = 0; i < ports_index; i++) {
        if (strcmp(name, ports[i].conf->name) == 0) {
            spinlock_unlock(PORT_LIST_SPINLOCK);
            return true;
        }
    }
    spinlock_unlock(PORT_LIST_SPINLOCK);
    return false;
}

u8 port_name2id(char *name) {
    u8 i;
    spinlock_lock(PORT_LIST_SPINLOCK);
    for (i = 0; i < ports_index; i++) {
        if (strcmp(name, ports[i].conf->name) == 0) {
            spinlock_unlock(PORT_LIST_SPINLOCK);
            return i;
        }
    }
    spinlock_unlock(PORT_LIST_SPINLOCK);
    return 0xff;
}

