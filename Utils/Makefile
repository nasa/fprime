# This Makefile goes in each module, and allows building of an individual module library.
# It is expected that each developer will add targets of their own for building and running
# tests, for example.

# derive module name from directory

MODULES = Hash

BASE_DIR = $(notdir $(CURDIR))

BUILD_ROOT ?= $(subst /$(BASE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/base_targets.mk
