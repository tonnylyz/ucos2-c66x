CG_TOOL_ROOT := C:/ti/ti-cgt-c6000_8.3.3

LINKER_CMD := task.cmd

OBJS += task.obj stub.obj

INCLUDE_PATH := --include_path="$(CURDIR)"

LIBRARY_PATH := -i $(CG_TOOL_ROOT)/lib

CL6X_FLAGS := -mv6600 --opt_level=0 --define=SOC_AM572x --define=am5728 --diag_warning=225 --diag_wrap=off --display_error_number --call_assumptions=0

TARGET_NAME := task.elf

%.obj: %.c
	@echo $<
	@$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --mem_model:data=far --preproc_with_compile --output_file $@ $<


%.obj: %.asm
	@echo $<
	@$(CG_TOOL_ROOT)/bin/cl6x $(INCLUDE_PATH) $(CL6X_FLAGS) --mem_model:data=far --preproc_with_compile --output_file $@ $<


all: $(TARGET_NAME)

$(TARGET_NAME): $(OBJS)
	@$(CG_TOOL_ROOT)/bin/cl6x $(LIBRARY_PATH) $(CL6X_FLAGS) -z -m "task.map" --reread_libs --warn_sections --ram_model -o $(TARGET_NAME) $(OBJS) $(LINKER_CMD)

clean:
	-rm -rf $(TARGET_NAME)
	-rm -rf task.map
	-rm -rf *.obj

dis: $(TARGET_NAME)
	$(CG_TOOL_ROOT)/bin/dis6x $(TARGET_NAME) > task.dis

win: $(TARGET_NAME)
	cp task.elf E:/

.PHONY: all clean dissemble
