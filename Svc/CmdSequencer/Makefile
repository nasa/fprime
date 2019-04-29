# ---------------------------------------------------------------------- 
# Makefile
# ---------------------------------------------------------------------- 

MODULE_DIR = Svc/CmdSequencer
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk

clean : clean_extra

clean_extra :
	rm -rf test/ut/bin

ut : ut_extra

ut_clean : ut_clean_extra

ut_extra :
	mkdir -p test/ut/bin

ut_clean_extra :
	rm -rf test/ut/bin

.PHONY : clean_extra ut_clean_extra ut_extra
