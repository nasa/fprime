TEST_SRC = Main.cpp

TEST_MODS = Fw/FilePacket/GTest \
Fw/Types/GTest \
Fw/FilePacket \
Fw/Buffer \
Fw/Port \
Fw/Comp \
Os \
Fw/Obj \
Utils/Hash \
CFDP/Checksum/GTest \
CFDP/Checksum \
Fw/Types \
gtest



COMPARGS = -I$(CURDIR)/test/ut/Handcode
