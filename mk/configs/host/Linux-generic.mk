include $(BUILD_ROOT)/mk/configs/host/Linux-common.mk

export PYTHON_BASE ?= /usr
export PYTHON_BIN := $(PYTHON_BASE)/bin/python
export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(PYTHON_BASE)/lib

# This assumes that you have installed lxml and cheetah to the python distribution. Probably requires sudo access.
export LXML_PATH := 
export CHEETAH_COMPILE ?= cheetah-compile
export MARKDOWN ?= $(PYTHON_BASE)/bin/markdown_py -x markdown.extensions.extra -x markdown.extensions.codehilite

CKSUM_SH := $(BUILD_ROOT)/mk/bin/md5.sh
export CKSUM := /usr/bin/openssl md5
CKSUM_SUFFIX := .md5

DOXYGEN := 

JOBS := -j 4
#JOBS =

-include $(BUILD_ROOT)/mk/configs/target/wind-$(TARGET).mk

CRC := $(BUILD_ROOT)/mk/bin/run_file_crc.sh


export PI_TOOLS := /opt/tools/rpi


SYMBOL_CHECK := echo