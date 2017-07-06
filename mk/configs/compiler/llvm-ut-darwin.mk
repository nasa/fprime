include $(BUILD_ROOT)/mk/configs/compiler/darwin_llvm_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk

CFLAGS := 	$(DEBUG) \
			$(DARWIN_LLVM_CFLAGS) \
			$(DARWIN_LLVM_INCLUDES) \
			$(COVERAGE) \
			$(UT_FLAGS)

CXXFLAGS := $(DEBUG) \
			$(DARWIN_LLVM_CXXFLAGS) \
			$(DARWIN_LLVM_INCLUDES) \
			$(COVERAGE) \
			$(UT_FLAGS)
			
LINK_BIN_FLAGS := $(LINK_BIN_FLAGS) $(COVERAGE)			

COMPILER := darwin-x86-ut-llvm
