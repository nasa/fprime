# Makefile to run global make.
DEPLOYMENT := RPI
BUILD_ROOT ?= $(subst /$(DEPLOYMENT),,$(CURDIR))

export BUILD_ROOT

default: RASPIAN dict_install

rebuild_rpi: gen_make RASPIAN_clean RASPIAN dict_install

build_dict: gen_make ac_lvl4 dict_install

rebuild_dict: gen_make clean build_dict

include $(BUILD_ROOT)/mk/makefiles/deployment_makefile.mk

