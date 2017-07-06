HOST_BIN_SUFFIX = 
MKDIR := mkdir -p
export RM := rm -f
RM_DIR = $(RM) -rf 
LS := ls
SIZE := size
LS_SIZE := -hs
export CAT := cat
TIME := /usr/bin/time -f"Build Time: %E"
CP := cp
CP_RECURSE := $(CP) -r
TEE := tee
CHMOD_WRITE_USER := chmod -f u+w
TOUCH := touch
NM := /usr/bin/nm
MAKE := /usr/bin/make

PERL_BASE := /usr
export PERL_BIN := $(PERL_BASE)/bin/perl
export PERL_LIB := $(PERL_BASE)/lib

SLOC_COUNTER := ${BUILD_ROOT}/mk/bin/ncsl -v -i
PROCESS_SLOC := ${BUILD_ROOT}/mk/bin/process_ncsl_sloc.py
FILE_HASH := $(PYTHON_BIN) ${BUILD_ROOT}/mk/bin/run_file_hash.py
GEN_VERSION := $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/gen_git_version.py
