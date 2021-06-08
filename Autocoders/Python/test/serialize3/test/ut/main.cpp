#include <Autocoders/Python/test/serialize3/AlltypesSerializableAc.hpp>

#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include "Fw/Test/UnitTestAssert.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <string.h>

TEST(Serialize3, DefaultInit) {
	// Create Alltypes serializable struct
	Ns::Something::Alltypes atypes;

	// Check that default values were initialized
	ASSERT_EQ(atypes.geti8(), 43);
	ASSERT_EQ(atypes.getf64(), 1.2345);
	ASSERT_EQ(atypes.getstr(), "hello");
	ASSERT_EQ(atypes.getb(), true);
}


int main(int argc, char* argv[]) {
     ::testing::InitGoogleTest(&argc, argv);
     int status = RUN_ALL_TESTS();
     return status;
}


