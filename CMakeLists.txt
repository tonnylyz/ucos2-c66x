project(ucos2-c66x)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_FLAGS "-D DSP_CORE_1")

include_directories(
        boot
        kernel
        klib
        platform/c66x
        platform/csp
        task
)

add_executable(
        kern

        boot/main.c
        boot/resource_table.h

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

        platform/c66x/os_cpu.h
        platform/c66x/os_cpu_c.c

        platform/csp/types.h
        platform/csp/timer.c platform/csp/timer.h
        platform/csp/uart.c platform/csp/uart.h
        platform/csp/mmio.h

        klib/printf.c
        klib/printf.h

        task/part_0.c
        task/part_0.h

        platform/csp/intc.c platform/csp/intc.h task/syscall.c task/syscall.h platform/csp/xmc.c platform/csp/xmc.h klib/partition.c klib/partition.h klib/partition_conf.h)