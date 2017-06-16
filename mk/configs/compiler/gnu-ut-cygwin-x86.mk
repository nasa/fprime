include $(BUILD_ROOT)/mk/configs/compiler/cygwin_gnu_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/ut_flags.mk

CFLAGS := 	$(DEBUG) \
			$(LINUX_GNU_CFLAGS) \
			$(LINUX_GNU_INCLUDES) \
			$(COVERAGE) \
			$(UT_FLAGS)

CXXFLAGS := $(DEBUG) \
			$(LINUX_GNU_CXXFLAGS) \
			$(LINUX_GNU_INCLUDES) \
			$(COVERAGE) \
			$(UT_FLAGS)
			
LINK_BIN_FLAGS := $(LINK_BIN_FLAGS) $(COVERAGE)

COMPILER := cygwin-x86-ut-gnu
