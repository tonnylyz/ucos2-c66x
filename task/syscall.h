#ifndef UCOS2_C66X_SYSCALL_H
#define UCOS2_C66X_SYSCALL_H

#include <types.h>

void putc(char c);

void puts(char *str);

/* syscall ~ os_time.c */

void time_delay(u32 ticks);

int time_delay_hmsm(u8 h, u8 m, u8 s, u16 ms);

int time_delay_resume(u8 priority);

u32 time_get();

void time_set(u32 ticks);

/* syscall ~ os_task.c */

int task_task_change_priority(u8 oldprio, u8 newprio);

int task_delete(u8 prio);

int task_delete_request(u8 prio);

int task_resume(u8 prio);

int task_stack_check(u8 prio, void *p_stk_data);

int task_suspend(u8 prio);

int task_query(u8 prio, void* p_task_data);

u32 task_reg_get(u8 prio, u8 id, void *perr);

void task_reg_set(u8 prio, u8 id, u32 value, void* perr);

#endif //UCOS2_C66X_SYSCALL_H
