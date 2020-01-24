# Makefile to run global make.
DEPLOYMENT := GpsApp
BUILD_ROOT ?= $(subst /$(DEPLOYMENT),,$(CURDIR))

export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/deployment_makefile.mk

rebuild_rpi_cross: gen_make rpi_cross_clean rpi_cross

rpi_cross: ac_lvl4 dict_install
	@$(TIME) $(MAKE) $(JOBS) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) COMP=gcc-cross BUILD=RASPIAN bin

rpi_cross_clean:
	@$(TIME) $(MAKE) $(JOBS) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) COMP=gcc-cross BUILD=RASPIAN clean

