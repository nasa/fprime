TEST_SRC = 	Handcode/GTestBase.cpp \
			Handcode/TesterBase.cpp \
			Tester.cpp \
			main.cpp

TEST_MODS = Autocoders/Python/test/serial_passive \
			Fw/Cmd Fw/Comp \
			Fw/Port Fw/Prm \
			Fw/Time Fw/Tlm \
			Fw/Types Fw/Log \
			Fw/Obj Os Fw/Com \
			Autocoders/Python/test/port_loopback



COMPARGS = -I$(CURDIR)/test/ut/Handcode
