include $(BUILD_ROOT)/mk/configs/compiler/darwin_llvm_common.mk
include $(BUILD_ROOT)/mk/configs/compiler/include_common.mk

CFLAGS := 	$(DEBUG) \
			$(DARWIN_LLVM_CFLAGS) \
			$(DARWIN_LLVM_INCLUDES) \
			$(UT_FLAGS)

CXXFLAGS := $(DEBUG) \
			$(DARWIN_LLVM_CXXFLAGS) \
			$(DARWIN_LLVM_INCLUDES) \
			$(UT_FLAGS)
			
COMPILER := darwin-x86-ut-nocov-llvm
