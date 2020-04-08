# This Makefile goes in each module, and allows building of an individual module library.
# It is expected that each developer will add targets of their own for building and running
# tests, for example.

# derive module name from directory

MODULE_DIR = Drv/LinuxI2cDriver
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk

# Add module specific targets here

ut_raspian_cross:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=gcc-cross BUILD=RASPIAN test_$(MODULE)testut
	
ut_raspian_cross_clean:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=gcc-cross BUILD=RASPIAN test_$(MODULE)testut_clean
	