//
// Created by Tonny on 3/24/2019.
//

#ifndef UCOS2_C66X_MAILBOX_H
#define UCOS2_C66X_MAILBOX_H

#include "types.h"

#define MAILBOX_1_IRQENABLE_SET 0x4A0F4108
#define MAILBOX_1_IRQSTATUS_CLR 0x4A0F4104
#define MAILBOX_1_MESSAGE 0x4A0F4040

#define MAILBOX_2_IRQENABLE_SET 0x4883A108
#define MAILBOX_2_IRQSTATUS_CLR 0x4883A104
#define MAILBOX_2_MESSAGE 0x4883A040

void mailbox_init();

void mailbox_send(u32 value);

u32 mailbox_receive();

void mailbox_irq_clear();

#endif //UCOS2_C66X_MAILBOX_H
