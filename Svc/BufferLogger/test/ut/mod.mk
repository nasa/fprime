# ----------------------------------------------------------------------
# mod.mk
# ----------------------------------------------------------------------

TEST_SRC=TesterBase.cpp \
				 GTestBase.cpp \
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
