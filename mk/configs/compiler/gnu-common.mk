GNU_COMMON := -Wall -Wextra \
               -fno-builtin -fno-asm \
               -fstrength-reduce \
               -Wno-unused-parameter \
               -Wno-long-long
               
DEPEND_FILE := -MMD -MP -MF

GNU_CFLAGS_COMMON := $(GNU_COMMON) -std=c99 \
				-fno-cond-mismatch -pedantic \
				-Werror-implicit-function-declaration \
				-Wstrict-prototypes

GNU_CXXFLAGS_COMMON := $(GNU_COMMON)  -fcheck-new \
				-Wnon-virtual-dtor 

COMPILE_ONLY := -c
SHARED_LIBRARY :=  -fPIC

COMPILE_TO := -o
LIBRARY_TO := -o
LINK_BIN_PRE_TO := -o
LINK_BIN_POST_TO := -o
LINK_BIN_TO := -o

POST_LINK_BIN := @echo

LIBS_START := -Wl,--start-group
LIBS_END := -Wl,--end-group

INCLUDE_PATH := -I

DEBUG := -g3
OPT_SPEED := -Os
OPT_NONE := -O0

SYMBOL_CHECK := 