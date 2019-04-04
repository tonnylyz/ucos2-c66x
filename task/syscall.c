#include "syscall.h"

#pragma SET_CODE_SECTION(".text:PART_S")
#pragma SET_DATA_SECTION(".data:PART_S")

extern u32 task_syscall(u32 a0, u32 a1, u32 a2, u32 a3,
                        u32 a4, u32 a5, u32 a6, u32 a7, u32 no);

void putc(char c) {
    task_syscall((u32) c, 0, 0, 0, 0, 0, 0, 0, 0);
}

void puts(char *str) {
    task_syscall((u32) str, 0, 0, 0, 0, 0, 0, 0, 1);
}

void time_delay(u32 ticks) {
    task_syscall(ticks, 0, 0, 0, 0, 0, 0, 0, 2);
}

int time_delay_hmsm(u8 h, u8 m, u8 s, u16 ms) {
    return (int) task_syscall(h, m, s, ms, 0, 0, 0, 0, 3);
}

int time_delay_resume(u8 priority) {
    return (int) task_syscall(priority, 0, 0, 0, 0, 0, 0, 0, 4);
}

u32 time_get() {
    return task_syscall(0, 0, 0, 0, 0, 0, 0, 0, 5);
}

void time_set(u32 ticks) {
    task_syscall(ticks, 0, 0, 0, 0, 0, 0, 0, 6);
}

int task_change_priority(u8 oldprio, u8 newprio) {
    return (int) task_syscall(oldprio, newprio, 0, 0, 0, 0, 0, 0, 7);
}

int task_delete(u8 prio) {
    return (int) task_syscall(prio, 0, 0, 0, 0, 0, 0, 0, 8);
}

int task_delete_request(u8 prio) {
    return (int) task_syscall(prio, 0, 0, 0, 0, 0, 0, 0, 9);
}

int task_resume(u8 prio) {
    return (int) task_syscall(prio, 0, 0, 0, 0, 0, 0, 0, 10);
}

int task_stack_check(u8 prio, void *p_stk_data) {
    return (int) task_syscall(prio, (u32) p_stk_data, 0, 0, 0, 0, 0, 0, 11);
}

int task_suspend(u8 prio) {
    return (int) task_syscall(prio, 0, 0, 0, 0, 0, 0, 0, 12);
}

int task_query(u8 prio, void *p_task_data) {
    return (int) task_syscall(prio, (u32) p_task_data, 0, 0, 0, 0, 0, 0, 13);
}

u32 task_reg_get(u8 prio, u8 id, void *perr) {
    return task_syscall(prio, id, (u32) perr, 0, 0, 0, 0, 0, 14);
}

void task_reg_set(u8 prio, u8 id, u32 value, void *perr) {
    task_syscall(prio, id, value, (u32) perr, 0, 0, 0, 0, 15);
}

u32 task_ipc_receive() {
    return task_syscall(0, 0, 0, 0, 0, 0, 0, 0, 16);
}

int task_ipc_send(u8 prio, u32 value) {
    return (int) task_syscall(prio, value, 0, 0, 0, 0, 0, 0, 17);
}

u32 task_ipc_receive_foreign(u8 pid, u32 addr, u32 max_len) {
    return task_syscall(pid, addr, max_len, 0, 0, 0, 0, 0, 18);
}

int task_ipc_send_foreign(u8 pid, u8 prio, u32 value, u32 addr, u32 len) {
    return (int) task_syscall(pid, prio, value, addr, len, 0, 0, 0, 19);
}

void u_apex_set_partition_mode(operating_mode_t ps, return_code_t *r) {
    task_syscall(ps, (u32) r, 0, 0, 0, 0, 0, 0, 20);
}

void u_apex_get_partition_mode(operating_mode_t *pps, return_code_t *r) {
    task_syscall((u32) pps, (u32) r, 0, 0, 0, 0, 0, 0, 21);
}

void u_apex_get_process_id(char *name, process_id_t *ppid, return_code_t *r) {
    task_syscall((u32) name, (u32) ppid, (u32) r, 0, 0, 0, 0, 0, 22);
}

void u_apex_get_process_status(process_id_t id, process_status_t *pps, return_code_t *r) {
    task_syscall(id, (u32) pps, (u32) r, 0, 0, 0, 0, 0, 23);
}

void u_apex_create_process(const process_attribute_t *ppa, process_id_t *ppid, return_code_t *r) {
    task_syscall((u32) ppa, (u32) ppid, (u32) r, 0, 0, 0, 0, 0, 24);
}

void u_apex_set_priority(process_id_t pid, u8 priority, return_code_t *r) {
    task_syscall(pid, priority, (u32) r, 0, 0, 0, 0, 0, 25);
}

void u_apex_suspend_self(system_time_t time_out, return_code_t *r) {
    task_syscall(time_out, (u32) r, 0, 0, 0, 0, 0, 0, 26);
}

void u_apex_stop_self(void) {
    task_syscall(0, 0, 0, 0, 0, 0, 0, 0, 27);
}

void u_apex_get_my_id(process_id_t *ppid, return_code_t *r) {
    task_syscall((u32) ppid, (u32) r, 0, 0, 0, 0, 0, 0, 28);
}
