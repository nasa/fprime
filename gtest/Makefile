# This Makefile should be placed in the module that is contained under another root directory. 
# The relative path from the root of the sandbox should be put in MODULE_DIR

# derive module name from directory

MODULE_DIR = gtest
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk

# Add module specific targets here