COMP ?= DEFAULT

ifeq ($(COMP),DEFAULT)
 include $(BUILD_ROOT)/mk/configs/builds/linux-gcc-debug.mk
endif

ifeq ($(COMP),gcc-debug)
 include $(BUILD_ROOT)/mk/configs/builds/linux-gcc-debug.mk
endif

ifeq ($(COMP),comp-ut)
 include $(BUILD_ROOT)/mk/configs/builds/linux-gcc-ut.mk
endif

ifeq ($(COMP),comp-ut-nocov)
 include $(BUILD_ROOT)/mk/configs/builds/linux-gcc-ut-nocov.mk
endif

ifeq ($(COMP),comp-integ)
 include $(BUILD_ROOT)/mk/configs/builds/linux-gcc-ut-nocov.mk
endif
