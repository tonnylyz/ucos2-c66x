CG_TOOL_ROOT := /home/tonny/ti/ccs820/ccsv8/tools/compiler/ti-cgt-c6000_8.2.4

LINKER_CMD := linker.cmd


# Bootstrap Objects
BSP_OBJS := boot/main.obj

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.obj  kernel/os_flag.obj kernel/os_mbox.obj kernel/os_mem.obj \
               kernel/os_mutex.obj kernel/os_q.obj    kernel/os_sem.obj  kernel/os_task.obj \
               kernel/os_time.obj  kernel/os_tmr.obj  kernel/os_dbg.obj


OBJS += $(BSP_OBJS)

INCLUDE_PATH := --include_path="$(CURDIR)" \
                --include_path="$(CURDIR)/boot" \
                --include_path="$(CG_TOOL_ROOT)/include"

LIBRARY_PATH := -i $(CG_TOOL_ROOT)/lib

CL6X_FLAGS := -mv6600 -O2 --define=am5728 --define=core1 --diag_warning=225 --diag_wrap=off --display_error_number

%.obj: %.c
	$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --preproc_with_compile --output_file $@ $<

all: dra7-dsp1-fw.xe66

dra7-dsp1-fw.xe66: $(OBJS)
	$(CG_TOOL_ROOT)/bin/cl6x $(LIBRARY_PATH) $(CL6X_FLAGS) -z -m "linker.map" --reread_libs --define=CORE1=1 --warn_sections --rom_model -o dra7-dsp1-fw.xe66 $(OBJS) $(LINKER_CMD)

clean:
	-rm -rf dra7-dsp1-fw.xe66
	-rm -rf linker.map
	-rm -rf *.obj
	-rm -rf */*.obj

.PHONY: all clean