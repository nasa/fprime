include $(BUILD_ROOT)/mk/configs/host/Linux-common.mk

TIME := time
PYTHON_BASE ?= /usr
export PYTHON_BASE # needed by ac scripts
export PYTHON_BIN := $(PYTHON_BASE)/bin/python

export CHEETAH_COMPILE ?= cheetah-compile
export MARKDOWN ?= markdown_py -x markdown.extensions.extra -x markdown.extensions.codehilite

FILE_HASH := $(BUILD_ROOT)/mk/bin/run_file_hash.sh

SIZE = size
CKSUM_SH := $(BUILD_ROOT)/mk/bin/md5.sh
export CKSUM := /sbin/md5

MAKE = make
JOBS = -j 4
#JOBS =
