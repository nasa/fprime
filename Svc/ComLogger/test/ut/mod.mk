# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC=Handcode/TesterBase.cpp \
				 Handcode/GTestBase.cpp \
				 Tester.cpp \
				 Main.cpp 

TEST_MODS=Svc/ComLogger \
			Svc/Ping \
					Fw/Cmd Fw/Comp Fw/Port Fw/Prm Fw/Time \
					Fw/Tlm Fw/Types Fw/Log Fw/Obj Os Fw/Com \
					Utils/Hash \
					gtest




COMPARGS = -I$(CURDIR)/test/ut/Handcode
