# Makefile to run global make.
DEPLOYMENT := RPI
BUILD_ROOT ?= $(subst /$(DEPLOYMENT),,$(CURDIR))

export BUILD_ROOT

default: RASPIAN dict_install

rebuild_rpi: gen_make RASPIAN_clean RASPIAN dict_install

build_dict: gen_make ac_lvl4 dict_install

rebuild_dict: gen_make clean build_dict

include $(BUILD_ROOT)/mk/makefiles/deployment_makefile.mk

rebuild_rpi_cross: gen_make rpi_cross_clean rpi_cross

rpi_cross: ac_lvl4 dict_install
	@$(TIME) $(MAKE) $(JOBS) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) COMP=gcc-cross BUILD=RASPIAN bin

rpi_cross_clean:
	@$(TIME) $(MAKE) $(JOBS) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) COMP=gcc-cross BUILD=RASPIAN clean

