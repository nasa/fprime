include $(BUILD_ROOT)/mk/configs/compiler/linux_gnu_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk

CFLAGS := 	$(DEBUG) \
			$(LINUX_GNU_CFLAGS) \
			$(LINUX_GNU_INCLUDES)
			

CXXFLAGS := $(DEBUG) \
			$(LINUX_GNU_CXXFLAGS) \
			$(LINUX_GNU_INCLUDES)

COMPILER := linux-x86-debug-gnu
