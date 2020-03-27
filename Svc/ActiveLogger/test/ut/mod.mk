#
#   Copyright 2004-2008, by the California Institute of Technology.
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

TEST_SRC = 	Handcode/TesterBase.cpp \
			Handcode/GTestBase.cpp \
			ActiveLoggerTester.cpp \
			ActiveLoggerImplTester.cpp

TEST_MODS = Svc/ActiveLogger \
			Svc/Sched \
			Svc/Fatal \
			Svc/Ping \
			Fw/Tlm \
			Fw/Cmd \
			Fw/Comp \
			Fw/Log \
			Fw/Prm \
			Fw/Com \
			Fw/Time \
			Fw/Obj \
			Fw/Port \
			Fw/Types \
			Os/Stubs \
			Os \
			gtest



COMPARGS = -I$(CURDIR)/test/ut/Handcode
