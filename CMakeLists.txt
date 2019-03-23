project(ucos2-c66x)
cmake_minimum_required(VERSION 3.10)

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
        task/part_1.c
        task/part_1.h
        task/part_3.c
        task/part_3.h
        task/part_2.c
        task/part_2.h

        platform/csp/intc.c platform/csp/intc.h task/syscall.c task/syscall.h platform/csp/xmc.c platform/csp/xmc.h klib/partition.c klib/partition.h klib/partition_conf.h klib/isr.c klib/isr.h platform/csp/spinlock.c platform/csp/spinlock.h klib/apex.c klib/apex.h klib/def.h klib/ipc.c klib/ipc.h)