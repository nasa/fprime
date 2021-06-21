#include <Autocoders/Python/test/serialize4/Serial1SerializableAc.hpp>

#include "gtest/gtest.h"

using namespace std;

TEST(EnumXML, DefaultValues) {

    Example::Serial1 serial1;

    // Check serializable default values are correctly set
    ASSERT_EQ(serial1.getMember1(), 12345);
    ASSERT_EQ(serial1.getMember2(), "hello");
    ASSERT_EQ(serial1.getMember3(), Example::MEM2);

}

TEST(EnumXML, ArrayScalarInit) {

    Example::Serial1 serial1 (0,0,Example::Enum1::Item2,0);

    // Check serializable default values are correctly set
    ASSERT_EQ(serial1.getMember4(), 12345);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    int status = RUN_ALL_TESTS();

    cout << "Completed..." << endl;

    return status;
}
