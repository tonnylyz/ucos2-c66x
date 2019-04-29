project(ucos2-c66x)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)
add_compile_definitions(SOC_AM572x)

include_directories(
        kernel
        klib
        platform/c66x
        platform/driver
        ulib
        fatfs
        C:/ti/pdk_am57xx_1_0_14/packages
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
        platform/driver/xmc.c
        platform/driver/xmc.h

        klib/printf.c
        klib/printf.h
        klib/isr.c

        ulib/syscall.c
        ulib/syscall.h

        user/task.c
        platform/driver/mmc.c
        platform/driver/mmc.h
        klib/mbr.c
        klib/mbr.h
        klib/fat32.c
        klib/fat32.h

        fatfs/ff.c
        fatfs/ff.h
        fatfs/ffconf.h
        fatfs/diskio.c
        fatfs/diskio.h
        klib/elf.c klib/elf.h user/stub.c user/stub.h)