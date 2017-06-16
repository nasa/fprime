include $(BUILD_ROOT)/mk/configs/host/compute_host.mk
include $(BUILD_ROOT)/mk/configs/host/compute_build_site.mk
include $(BUILD_ROOT)/mk/configs/host/$(HOST)-$(BUILD_SITE).mk
include $(BUILD_ROOT)/mk/configs/builds/builds.mk

all: $(NATIVE_BUILD)

$(BUILDS):
	for module in $(MODULES); do \
		make -f $$module/Makefile $@; \
	done