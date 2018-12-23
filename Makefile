CG_TOOL_ROOT := /home/tonny/ti/ccs820/ccsv8/tools/compiler/ti-cgt-c6000_8.2.4

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
                 platform/csp/intc_s.obj \
                 platform/c66x/os_cpu_a.obj \
                 platform/c66x/os_cpu_c.obj

# Task Objects
TASK_OBJS := task/app.obj \
             task/syscall.obj \
             task/syscall_s.obj

# Kernel Library Objects
KLIB_OBJS := klib/printf.obj

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

CL6X_FLAGS := -mv6600 --opt_level=0 --define=SOC_AM572x --define=am5728 --define=core1 --diag_warning=225 --diag_wrap=off --display_error_number --call_assumptions=0

%.obj: %.c
	$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --preproc_with_compile --output_file $@ $<


%.obj: %.asm
	$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --preproc_with_compile --output_file $@ $<


all: dra7-dsp1-fw.xe66

dra7-dsp1-fw.xe66: $(OBJS)
	$(CG_TOOL_ROOT)/bin/cl6x $(LIBRARY_PATH) $(CL6X_FLAGS) -z -m "linker.map" --reread_libs --define=CORE1=1 --warn_sections --rom_model -o dra7-dsp1-fw.xe66 $(OBJS) $(LINKER_CMD)

clean:
	-rm -rf dra7-dsp1-fw.xe66
	-rm -rf linker.map
	-rm -rf *.obj
	-rm -rf */*.obj
	-rm -rf platform/*/*.obj

dis: dra7-dsp1-fw.xe66
	$(CG_TOOL_ROOT)/bin/dis6x dra7-dsp1-fw.xe66 > dis.out

burn: dra7-dsp1-fw.xe66
	cp dra7-dsp1-fw.xe66 /media/tonny/BOOT/dra7-dsp1-fw.xe66
	sync
	umount /dev/sdc1
	udisksctl power-off -b /dev/sdc

.PHONY: all clean dis burn