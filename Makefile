CG_TOOL_ROOT := C:/ti/ti-cgt-c6000_8.3.3
PDK_ROOT := C:/ti/pdk_am57xx_1_0_14

LINKER_CMD := linker.cmd

# Bootstrap Objects
BSP_OBJS := boot/vector.obj boot/main.obj

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.obj  kernel/os_flag.obj kernel/os_mbox.obj kernel/os_mem.obj \
               kernel/os_mutex.obj kernel/os_q.obj    kernel/os_sem.obj  kernel/os_task.obj \
               kernel/os_time.obj  kernel/os_tmr.obj  kernel/os_dbg.obj

# Platform Objects
PLATFORM_OBJS := platform/driver/timer.obj \
                 platform/driver/uart.obj \
                 platform/driver/intc.obj \
                 platform/driver/xmc.obj \
                 platform/driver/mmc.obj \
                 platform/c66x/os_cpu_a.obj \
                 platform/c66x/os_cpu_c.obj

# Kernel Library Objects
KLIB_OBJS := klib/printf.obj \
             klib/isr.obj \
             klib/mbr.obj \
             klib/fat32.obj \
             klib/elf.obj \
             fatfs/diskio.obj \
             fatfs/ff.obj

# User Library Objects
ULIB_OBJS := ulib/syscall.obj ulib/syscall_s.obj

OBJS += $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) $(KLIB_OBJS) $(ULIB_OBJS)

INCLUDE_PATH := --include_path="$(CURDIR)" \
                --include_path="$(CURDIR)/boot" \
                --include_path="$(CURDIR)/platform/driver" \
                --include_path="$(CURDIR)/platform/c66x" \
                --include_path="$(CURDIR)/kernel" \
                --include_path="$(CURDIR)/klib" \
                --include_path="$(CURDIR)/fatfs" \
                --include_path="$(PDK_ROOT)/packages" \
                --include_path="$(CG_TOOL_ROOT)/include"

LIBRARY_PATH := -i $(CG_TOOL_ROOT)/lib \
                -i $(PDK_ROOT)/packages/ti/csl/lib/am572x/c66/release

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
	-rm $(TARGET_NAME)
	-rm linker.map
	-rm boot/*.obj
	-rm fatfs/*.obj
	-rm kernel/*.obj
	-rm klib/*.obj
	-rm task/*.obj
	-rm platform/c66x/*.obj
	-rm platform/driver/*.obj

dis: $(TARGET_NAME)
	$(CG_TOOL_ROOT)/bin/dis6x dra7-dsp1-fw.xe66 > kernel.dis

win:
	cp dra7-dsp1-fw.xe66 E:/dra7-dsp1-fw.xe66
	RemoveDrive.exe E: -L

.PHONY: all clean dis win

