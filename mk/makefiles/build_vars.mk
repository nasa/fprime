include $(BUILD_ROOT)/mk/configs/host/compute_host.mk 
include $(BUILD_ROOT)/mk/configs/host/compute_build_site.mk

ifneq ("$(BUILD)","") # only include if building binary targets 
  include $(BUILD_ROOT)/mk/configs/builds/$(BUILD).mk
endif

include $(BUILD_ROOT)/mk/configs/host/$(HOST)-$(BUILD_SITE).mk

ifneq ("$(BUILD)","") # only include if building binary targets 
 include $(BUILD_ROOT)/mk/configs/compiler/$(COMPILER).mk
 include $(BUILD_ROOT)/mk/configs/target/$(TARGET).mk
endif

include $(BUILD_ROOT)/mk/configs/modules/modules.mk
include $(BUILD_ROOT)/mk/configs/compiler/actools.mk

print_output_dir:
	@echo $(OUTPUT_DIR)
	
print_gcov:
	@echo $(GCOV)
	
print_build_site:
	@echo $(BUILD_SITE)
	
print_native_build:
	@echo $(NATIVE_BUILD)
