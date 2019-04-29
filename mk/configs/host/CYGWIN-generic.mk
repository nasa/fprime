include $(BUILD_ROOT)/mk/configs/host/Linux-common.mk

export PYTHON_BIN := python
export CHEETAH_COMPILE ?= /bin/cheetah-compile
export MARKDOWN ?= /usr/bin/markdown_py -x markdown.extensions.extra -x markdown.extensions.codehilite
export DOXYGEN := doxygen 

HOST_BIN_SUFFIX := .exe
JOBS = -j 4

SYMBOL_CHECK := echo

FILE_HASH := $(PYTHON_BIN) ${BUILD_ROOT}/mk/bin/run_file_hash.py

export HOME := $(HOMEPATH)
export USER := $(USERNAME)
