//
// Created by Tonny on 3/23/2019.
//

#include <types.h>

#ifndef UCOS2_C66X_IPC_H
#define UCOS2_C66X_IPC_H


void ipc_receive();

void ipc_receive_foreign(u8 pid, u32 addr, u32 max_len);

int ipc_send(u8 prio, u32 value);

int ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len);

void ipc_scan_change();

#endif //UCOS2_C66X_IPC_H
