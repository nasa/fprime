#
#   Copyright 2004-2015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

TEST_SRC = 	Handcode/TesterBase.cpp \
			Handcode/GTestBase.cpp \
			Tester.cpp \
			main.cpp

TEST_MODS = Svc/Health \
			Svc/Ping \
			Svc/WatchDog \
			Fw/Tlm \
			Fw/Comp \
			Fw/Cmd \
			Fw/Log \
			Fw/Obj \
			Fw/Port \
			Fw/Time \
			Fw/Types \
			Fw/Com \
			Svc/Sched \
			Os \
			gtest



COMPARGS = -I$(CURDIR)/test/ut/Handcode
