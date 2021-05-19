TEST_SRC = Test.cpp 
TEST_MODS = Fw/SerializableFile \
	    Fw/SerializableFile/test/TestSerializable \
	    Fw/Types Utils/Hash \
	    gtest \
	    Os
	    



COMPARGS = -I$(CURDIR)/test/ut/Handcode
