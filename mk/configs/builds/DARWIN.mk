COMP ?= DEFAULT

ifeq ($(COMP),DEFAULT)
 include $(BUILD_ROOT)/mk/configs/builds/darwin-llvm-debug.mk
endif

ifeq ($(COMP),comp-ut)
 include $(BUILD_ROOT)/mk/configs/builds/darwin-llvm-ut.mk
endif

ifeq ($(COMP),comp-ut-nocov)
 include $(BUILD_ROOT)/mk/configs/builds/darwin-llvm-ut-nocov.mk
endif

ifeq ($(COMP),comp-integ)
 include $(BUILD_ROOT)/mk/configs/builds/darwin-llvm-ut-nocov.mk
endif

ifeq ($(COMP),llvm-debug)
 include $(BUILD_ROOT)/mk/configs/builds/darwin-llvm-debug.mk
endif
