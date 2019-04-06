#include "mailbox.h"
#include <mmio.h>

extern u8 core_id;

void mailbox_init(void) {
    if (core_id == 0) {
        mmio_write(MAILBOX_2_IRQENABLE_SET, 0b1);
    } else if (core_id == 1) {
        mmio_write(MAILBOX_1_IRQENABLE_SET, 0b1);
    }
}

void mailbox_send(u32 value) {
    if (core_id == 0) {
        mmio_write(MAILBOX_1_MESSAGE, value);
    } else if (core_id == 1) {
        mmio_write(MAILBOX_2_MESSAGE, value);
    }
}

u32 mailbox_receive(void) {
    if (core_id == 0) {
        return mmio_read(MAILBOX_2_MESSAGE);
    } else if (core_id == 1) {
        return mmio_read(MAILBOX_1_MESSAGE);
    }
    return 0;
}

void mailbox_irq_clear(void) {
    if (core_id == 0) {
        mmio_write(MAILBOX_2_IRQSTATUS_CLR, 0b1);
    } else if (core_id == 1) {
        mmio_write(MAILBOX_1_IRQSTATUS_CLR, 0b1);
    }
}
