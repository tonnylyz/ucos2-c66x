project(ucos2-c66x)
cmake_minimum_required(VERSION 3.12)

include_directories(
        boot
        kernel
)

add_executable(
        kern

        boot/main.c
        boot/resource_table.h
        boot/types.h

        kernel/os_core.c
        kernel/os_dbg.c
        kernel/os_flag.c
        kernel/os_mbox.c
        kernel/os_mem.c
        kernel/os_mutex.c
        kernel/os_q.c
        kernel/os_sem.c
        kernel/os_task.c
        kernel/os_time.c
        kernel/os_tmr.c
        kernel/ucos_ii.h
        kernel/os_cfg.h
)