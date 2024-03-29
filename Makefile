CG_TOOL_ROOT := /home/tonny/ti/ti-cgt-c6000_8.3.3

LINKER_CMD := linker.cmd

# Bootstrap Objects
BSP_OBJS := boot/vector.obj boot/main.obj

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.obj  kernel/os_flag.obj kernel/os_mbox.obj kernel/os_mem.obj \
               kernel/os_mutex.obj kernel/os_q.obj    kernel/os_sem.obj  kernel/os_task.obj \
               kernel/os_time.obj  kernel/os_tmr.obj  kernel/os_dbg.obj
# Platform Objects
PLATFORM_OBJS := platform/csp/timer.obj \
                 platform/csp/uart.obj \
                 platform/csp/intc.obj \
                 platform/csp/xmc.obj \
                 platform/csp/spinlock.obj \
                 platform/c66x/os_cpu_a.obj \
                 platform/c66x/os_cpu_c.obj

# Task Objects
TASK_OBJS := task/part_0.obj \
             task/part_1.obj \
             task/part_2.obj \
             task/part_3.obj \
             task/syscall.obj \
             task/syscall_s.obj

# Kernel Library Objects
KLIB_OBJS := klib/printf.obj \
             klib/isr.obj \
             klib/partition.obj

OBJS += $(BSP_OBJS) $(PLATFORM_OBJS) $(TASK_OBJS) $(UCOSII_OBJS) $(KLIB_OBJS)

INCLUDE_PATH := --include_path="$(CURDIR)" \
                --include_path="$(CURDIR)/boot" \
                --include_path="$(CURDIR)/platform/csp" \
                --include_path="$(CURDIR)/platform/c66x" \
                --include_path="$(CURDIR)/task" \
                --include_path="$(CURDIR)/kernel" \
                --include_path="$(CURDIR)/klib" \
                --include_path="$(CG_TOOL_ROOT)/include"

LIBRARY_PATH := -i $(CG_TOOL_ROOT)/lib

CL6X_FLAGS := -mv6600 --opt_level=0 --define=SOC_AM572x --define=am5728 --diag_warning=225 --diag_wrap=off --display_error_number --call_assumptions=0

TARGET_NAME := dra7-dsp1-fw.xe66

%.obj: %.c
	@echo $<
	@$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --preproc_with_compile --output_file $@ $<


%.obj: %.asm
	@echo $<
	@$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --preproc_with_compile --output_file $@ $<


all: $(TARGET_NAME)

$(TARGET_NAME): $(OBJS)
	@$(CG_TOOL_ROOT)/bin/cl6x $(LIBRARY_PATH) $(CL6X_FLAGS) -z -m "linker.map" --reread_libs --warn_sections --ram_model -o $(TARGET_NAME) $(OBJS) $(LINKER_CMD)

clean:
	-rm -rf $(TARGET_NAME)
	-rm -rf linker.map
	-rm -rf *.obj
	-rm -rf */*.obj
	-rm -rf platform/*/*.obj

dissemble: $(TARGET_NAME)
	$(CG_TOOL_ROOT)/bin/dis6x dra7-dsp1-fw.xe66 > dis.out

burn: dra7-dsp1-fw.xe66
	cp dra7-dsp1-fw.xe66 /media/tonny/BOOT/dra7-dsp1-fw.xe66
	sync
	umount /dev/sdc1
	udisksctl power-off -b /dev/sdc

win:
	sudo mount -t drvfs e: /mnt/e
	cp dra7-dsp1-fw.xe66 /mnt/e/dra7-dsp1-fw.xe66
	sync
	sleep 1
	sudo umount /mnt/e
	RemoveDrive.exe e: -L



.PHONY: all clean dissemble burn win

