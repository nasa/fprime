TEST_SRC = Test.cpp 
TEST_MODS = Fw/SerializableFile \
	    Fw/SerializableFile/test/TestSerializable \
	    Fw/Types \
	    gtest \
	    Os
	    



COMPARGS = -I$(CURDIR)/test/ut/Handcode
