#
#   Copyright 2015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

# This is a template for the mod.mk file that goes in each module
# and each module's subdirectories.
# With a fresh checkout, "make gen_make" should be invoked. It should also be
# run if any of the variables are updated. Any unused variables can 
# be deleted from the file.

# There are some standard files that are included for reference

SRC = # source common to all targets

SRC_CWSCP124 = # source only compiled for Curtiss-Wright SCP124
SRC_MXSCS750 = # source only compiled for Maxwell SCS750
SRC_SMP400K = # source only compiled for Space Micro P400K

SRC_WSTS = # source only compiled for WSTS
SRC_LINUX = # source only compiled for LINUX
SRC_DARWIN = # source only compiled for Mac OS Darwin

####

TEST_SRC = # test code common to all targets

TEST_SRC_CWSCP124 = #  test code  only compiled for Curtiss-Wright SCP124
TEST_SRC_MXSCS750 = #  test code  only compiled for Maxwell SCS750
TEST_SRC_SMP400K = #  test code  only compiled for Space Micro P400K

TEST_SRC_FLIGHT = # test source only compiled for FLIGHT
TEST_SRC_FLTS = # test source only compiled for FLTS

TEST_SRC_DARWIN = # test source only compiled for Mac OS Darwin
TEST_SRC_LINUX = # test source only compiled for Linux

####

TEST_MODS = # other modules to link all test code with

TEST_MODS_CWSCP124 = #  other modules to link for Curtiss-Wright SCP124
TEST_MODS_MXSCS750 = #  other modules to link for Maxwell SCS750
TEST_MODS_SMP400K = #  other modules to link for Space Micro P400K

TEST_MODS_WSTS = # other modules to link WSTS test code with
TEST_MODS_LINUX = # other modules to link LINUX test code with

####


TEST_LIBS = # other libraries to link all test code with

TEST_LIBS_CWSCP124 = #  other libraries to link for Curtiss-Wright SCP124
TEST_LIBS_MXSCS750 = #  other libraries to link for Maxwell SCS750
TEST_LIBS_SMP400K = #  other libraries to link for Space Micro P400K

TEST_LIBS_WSTS = # other libraries to link WSTS test code with
TEST_LIBS_LINUX = # other libraries to link LINUX test code with

####

SUBDIRS = # any subdirectories that need to be recursed into.

####

COMPARGS = # any arguments that should be passed to the compiler for this module

COMPARGS_CWSCP124 = # any arguments that should be passed to the compiler for this module only for Curtiss-Wright SCP124
COMPARGS_MXSCS750 = # any arguments that should be passed to the compiler for this module only for Maxwell SCS750
COMPARGS_SMP400K = # any arguments that should be passed to the compiler for this module only Space Micro P400K

COMPARGS_WSTS = # any arguments that should be passed to the compiler for this module only for WSTS
COMPARGS_LINUX = # any arguments that should be passed to the compiler for this module only for LINUX

####

COMPARGS_POST = # any arguments that should be passed to the compiler for this module at the end of the command line

COMPARGS_POST_CWSCP124 = # any arguments that should be passed to the compiler for this module only for Curtiss-Wright SCP124 at the end of the command line
COMPARGS_POST_MXSCS750 = # any arguments that should be passed to the compiler for this module only for Maxwell SCS750 at the end of the command line
COMPARGS_POST_SMP400K = # any arguments that should be passed to the compiler for this module only Space Micro P400K at the end of the command line

COMPARGS_POST_WSTS = # any arguments that should be passed to the compiler for this module only for WSTS at the end of the command line
COMPARGS_POST_LINUX = # any arguments that should be passed to the compiler for this module only for LINUX at the end of the command line
