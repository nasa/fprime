# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC= \
				TesterBase.cpp \
				GTestBase.cpp \
				Tester.cpp \
				Main.cpp  \
				FileBuffer.cpp

TEST_MODS= \
	Svc/FileDownlink \
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
