#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

TEST_SRC =      OsQueueTest.cpp \
	        TestMain.cpp \
	        IntervalTimerTest.cpp \
                OsValidateFileTest.cpp \
	        OsTaskTest.cpp \
                OsFileSystemTest.cpp

TEST_MODS = Os Fw/Obj Fw/Types Utils/Hash



COMPARGS = -I$(CURDIR)/test/ut/Handcode
