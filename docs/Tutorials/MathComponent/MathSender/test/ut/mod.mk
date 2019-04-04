# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC =       main.cpp \
				Tester.cpp \
				Handcode/GTestBase.cpp \
				Handcode/TesterBase.cpp

TEST_MODS =          Ref/MathSender \
					Ref/MathPorts \
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
