# ----------------------------------------------------------------------
# mod.mk
# ----------------------------------------------------------------------

TEST_SRC=Handcode/TesterBase.cpp \
				 Handcode/GTestBase.cpp \
				 Tester.cpp \
				 Main.cpp \
				 Errors.cpp \
				 Health.cpp \
				 Logging.cpp

TEST_MODS=Svc/BufferLogger \
					Fw/Buffer Fw/Cmd Fw/Comp Fw/Port Fw/Time \
					Fw/Tlm Fw/Types Fw/Log Fw/Obj Os Fw/Com \
					Svc/Ping Svc/Sched Utils/Hash \
					gtest



COMPARGS = -I$(CURDIR)/test/ut/Handcode
