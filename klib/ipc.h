#include <types.h>

#ifndef UCOS2_C66X_IPC_H
#define UCOS2_C66X_IPC_H

#define IPC_ERROR_DEST_TASK_NOT_EXISTS      -1
#define IPC_ERROR_DEST_TASK_NOT_RECEIVING   -2
#define IPC_ERROR_EXPECT_PID_MISMATCH       -3
#define IPC_ERROR_MESSAGE_TOO_LARGE         -4
#define IPC_ERROR_DEST_PART_NOT_EXISTS      -5
#define IPC_ERROR_ADDRESS_BOUNDARY          -6

void ipc_receive(void);

void ipc_receive_foreign(u8 pid, u32 addr, u32 max_len);

int ipc_send(u8 prio, u32 value);

int ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len);

void ipc_scan_change(void);

#endif //UCOS2_C66X_IPC_H
