project(ucos2-c66x)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS '-m32')
include_directories(
        boot
        kernel
        klib
        platform/c66x
        platform/driver
        task
        ulib
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

        platform/driver/mmio.h
        platform/driver/types.h
        platform/driver/timer.c
        platform/driver/timer.h
        platform/driver/uart.c
        platform/driver/uart.h
        platform/driver/intc.c
        platform/driver/intc.h
        platform/driver/mailbox.c
        platform/driver/mailbox.h
        platform/driver/xmc.c
        platform/driver/xmc.h
        platform/driver/spinlock.c
        platform/driver/spinlock.h

        klib/printf.c
        klib/printf.h
        klib/partition.c
        klib/partition.h
        klib/partition_conf.h
        klib/isr.c
        klib/apex.c
        klib/apex.h
        klib/def.h
        klib/ipc.c
        klib/ipc.h
        klib/port.c
        klib/port.h

        task/part_0.c
        task/part_1.c
        ulib/syscall.c
        ulib/syscall.h
        task/root.c)