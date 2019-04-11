
TEST_SRC = main.cpp \
	Tester.cpp \
	Handcode/GTestBase.cpp \
	Handcode/TesterBase.cpp
	
TEST_MODS = Svc/UdpSender \
			Svc/Sched \
			Fw/Tlm \
			Fw/Comp \
			Fw/Log \
			Fw/Obj \
			Fw/Port \
			Fw/Time \
			Fw/Buffer \
			Fw/Types \
			Os \
			gtest


COMPARGS = -I$(CURDIR)/test/ut/Handcode
