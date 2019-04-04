#ifndef UCOS2_C66X_SYSCALL_H
#define UCOS2_C66X_SYSCALL_H

#include <types.h>
#include <def.h>

void putc(char c);

void puts(char *str);

/* syscall ~ os_time.c */

void time_delay(u32 ticks);

int time_delay_hmsm(u8 h, u8 m, u8 s, u16 ms);

int time_delay_resume(u8 priority);

u32 time_get();

void time_set(u32 ticks);

/* syscall ~ os_task.c */

int task_change_priority(u8 oldprio, u8 newprio);

int task_delete(u8 prio);

int task_delete_request(u8 prio);

int task_resume(u8 prio);

int task_stack_check(u8 prio, void *p_stk_data);

int task_suspend(u8 prio);

int task_query(u8 prio, void* p_task_data);

u32 task_reg_get(u8 prio, u8 id, void *perr);

void task_reg_set(u8 prio, u8 id, u32 value, void* perr);

/* syscall ~ IPC */

u32 task_ipc_receive();

int task_ipc_send(u8 prio, u32 value);

u32 task_ipc_receive_foreign(u8 pid, u32 addr, u32 max_len);

int task_ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len);

/* syscall ~ APEX partition management */

void u_apex_set_partition_mode(operating_mode_t ps, return_code_t *r);

void u_apex_get_partition_mode(operating_mode_t *pps, return_code_t *r);

/* syscall ~ APEX process management */

void u_apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r);

void u_apex_get_process_status(process_id_t id, process_status_t *pps, return_code_t *r);

void u_apex_create_process(const process_attribute_t *ppa, process_id_t *ppid, return_code_t *r);

void u_apex_set_priority(process_id_t pid, u8 priority, return_code_t *r);

void u_apex_suspend_self(system_time_t time_out, return_code_t *r);

void u_apex_stop_self(void);

void u_apex_get_my_id(process_id_t *ppid, return_code_t *r);

#endif //UCOS2_C66X_SYSCALL_H
