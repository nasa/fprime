# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC= \
				Handcode/TesterBase.cpp \
				Handcode/GTestBase.cpp \
				Tester.cpp \
				Main.cpp  \
				FileBuffer.cpp

TEST_MODS= \
	Svc/FileDownlink \
	Svc/Ping \
	Fw/Tlm \
	Fw/Log \
	Fw/Time \
	Fw/FilePacket \
	Fw/Cmd \
	Fw/Com \
	Fw/Buffer \
	Fw/Port \
	Fw/Comp \
	Os \
	Fw/Obj \
	Utils/Hash \
	CFDP/Checksum \
	Fw/Types \
	gtest



COMPARGS = -I$(CURDIR)/test/ut/Handcode
