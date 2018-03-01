# Makefile to run global make.
DEPLOYMENT := RPI
BUILD_ROOT ?= $(subst /$(DEPLOYMENT),,$(CURDIR))

export BUILD_ROOT

default: RASPIAN dict_install

rebuild: gen_make RASPIAN_clean RASPIAN dict_install

include $(BUILD_ROOT)/mk/makefiles/deployment_makefile.mk

