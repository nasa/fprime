include $(BUILD_ROOT)/mk/configs/compiler/linux_gnu_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/ut_flags.mk

CFLAGS :=	$(DEBUG) \
			$(LINUX_GNU_CFLAGS) \
			$(LINUX_GNU_INCLUDES) \
			$(UT_FLAGS)

CXXFLAGS :=	$(DEBUG) \
			$(LINUX_GNU_CXXFLAGS) \
			$(LINUX_GNU_INCLUDES) \
			$(UT_FLAGS)

# for e.g. *Intercepter functions in Os/Stubs
LINK_BIN_FLAGS := -z muldefs $(LINK_BIN_FLAGS)

COMPILER := linux-x86-debug-gnu-ut-nocov
