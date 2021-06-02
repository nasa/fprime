# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC =       main.cpp \
				Tester.cpp \
				Handcode/GTestBase.cpp \
				Handcode/TesterBase.cpp

TEST_MODS =          Ref/MathReceiver \
					Ref/MathPorts \
					Ref/MathTypes \
					Svc/Sched \
					Fw/Cmd \
					Fw/Comp \
					Fw/Port \
					Fw/Prm \
					Fw/Time \
					Fw/Tlm \
					Fw/Types \
					Fw/Log \
					Fw/Obj \
					Fw/Com \
					Os \
					Utils/Hash \
					gtest




COMPARGS = -I$(CURDIR)/test/ut/Handcode
