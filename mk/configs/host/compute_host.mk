HOST = $(shell uname -s)

#adjust for various hosts
ifeq ("$(findstring CYGWIN,$(HOST))","CYGWIN") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    HOST = CYGWIN
    NATIVE_BUILD = $(HOST)
endif

ifeq ("$(HOST)","Linux")
    NATIVE_BUILD = LINUX
endif

ifeq ("$(HOST)","Darwin")
    NATIVE_BUILD = DARWIN
endif


HOSTNAME ?= $(shell uname -n)
