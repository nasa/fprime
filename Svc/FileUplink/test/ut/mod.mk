# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC=TesterBase.cpp \
				 GTestBase.cpp \
				 Tester.cpp \
				 Main.cpp 

TEST_MODS= \
	CFDP/Checksum \
	Svc/Ping \
	Fw/Buffer \
	Fw/Comp \
	Fw/FilePacket \
	Fw/Log \
	Fw/Obj \
	Fw/Port \
	Fw/Time \
	Fw/Tlm \
	Fw/Types \
	Utils/Hash \
	Os \
	Svc/FileUplink \
	gtest
