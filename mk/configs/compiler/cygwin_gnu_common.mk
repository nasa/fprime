include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/defines_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/linux_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/gnu-common.mk

CC :=  /usr/bin/gcc
CXX := /usr/bin/g++
GCOV := /usr/bin/gcov
AR := /usr/bin/ar

BUILD_32BIT := -m32

CC_MUNCH := $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/empty.py

LINK_LIB := $(AR)
LINK_LIB_FLAGS := rcs
LIBRARY_TO := 
POST_LINK_LIB := ranlib

LINK_BIN := $(CXX)
LINK_BIN_FLAGS := $(LIBS) $(BUILD_32BIT)

LINK_LIBS := -ldl -lpthread -lm -lrt

LINUX_GNU_CFLAGS := $(LINUX_FLAGS_COMMON) \
					$(COMMON_DEFINES) \
					$(GNU_CFLAGS_COMMON) \
					$(BUILD_32BIT) # Quantum framework won't build 64-bit

LINUX_GNU_CXXFLAGS :=	$(LINUX_FLAGS_COMMON) \
						$(COMMON_DEFINES) \
						$(GNU_CXXFLAGS_COMMON) \
						$(BUILD_32BIT)

COVERAGE := -fprofile-arcs -ftest-coverage

LINUX_GNU_INCLUDES := $(LINUX_INCLUDES_COMMON) $(COMMON_INCLUDES)


DUMP = $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/empty.py

MUNCH := $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/empty.py
