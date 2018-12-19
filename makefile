CG_TOOL_ROOT := /home/tonny/ti/ccs820/ccsv8/tools/compiler/ti-cgt-c6000_8.2.4

LINKER_CMD := linker.cmd


# Bootstrap Objects
BSP_OBJS := boot/main.obj

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.obj  kernel/os_flag.obj kernel/os_mbox.obj kernel/os_mem.obj \
               kernel/os_mutex.obj kernel/os_q.obj    kernel/os_sem.obj  kernel/os_task.obj \
               kernel/os_time.obj  kernel/os_tmr.obj  kernel/os_dbg.obj
# Platform Objects
PLATFORM_OBJS := platform/csp/timer.obj \
                 platform/csp/uart.obj \
                 platform/csp/intc.obj \
                 platform/c66x/os_cpu_a.obj platform/c66x/os_cpu_c.obj

# Task Objects
TASK_OBJS := task/app.obj task/snprintf.obj

OBJS += $(BSP_OBJS) $(PLATFORM_OBJS) $(TASK_OBJS) $(UCOSII_OBJS)

INCLUDE_PATH := --include_path="$(CURDIR)" \
                --include_path="$(CURDIR)/boot" \
                --include_path="$(CURDIR)/platform/csp" \
                --include_path="$(CURDIR)/platform/c66x" \
                --include_path="$(CURDIR)/task" \
                --include_path="$(CURDIR)/kernel" \
                --include_path="/home/tonny/ti/pdk_am57xx_1_0_12/packages" \
                --include_path="$(CG_TOOL_ROOT)/include"

LIBRARY_PATH := -i $(CG_TOOL_ROOT)/lib \
                -i /home/tonny/ti/pdk_am57xx_1_0_12/packages/ti/csl/lib/am572x/c66/release


CL6X_FLAGS := -mv6600 -O2 --define=SOC_AM572x --define=am5728 --define=core1 --diag_warning=225 --diag_wrap=off --display_error_number

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

dis:
	$(CG_TOOL_ROOT)/bin/dis6x dra7-dsp1-fw.xe66 > dis.out

.PHONY: all clean dis