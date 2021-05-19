# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC=Handcode/TesterBase.cpp \
				 Handcode/GTestBase.cpp \
				 Tester.cpp \
				 Main.cpp 

TEST_MODS=Svc/LinuxTime Svc/Time Fw/Time \
					Fw/Comp Fw/Types Fw/Obj \
                                        Fw/Port \
					Os \
					gtest




COMPARGS = -I$(CURDIR)/test/ut/Handcode
