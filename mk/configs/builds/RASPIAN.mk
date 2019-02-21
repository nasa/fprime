COMP ?= DEFAULT

ifeq ($(COMP),DEFAULT)
 include $(BUILD_ROOT)/mk/configs/builds/raspian-gcc-debug.mk
endif

ifeq ($(COMP),gcc-debug)
 include $(BUILD_ROOT)/mk/configs/builds/raspian-gcc-debug.mk
endif

ifeq ($(COMP),gcc-cross)
 include $(BUILD_ROOT)/mk/configs/builds/raspian-gcc-cross.mk
endif

ifeq ($(COMP),comp-ut)
 include $(BUILD_ROOT)/mk/configs/builds/raspian-gcc-ut.mk
endif

ifeq ($(COMP),comp-ut-nocov)
 include $(BUILD_ROOT)/mk/configs/builds/raspian-gcc-ut-nocov.mk
endif
