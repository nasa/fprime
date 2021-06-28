#include <Autocoders/Python/test/serialize4/Serial1SerializableAc.hpp>

#include "gtest/gtest.h"

using namespace std;

TEST(DefaultValues, OK) {

    Example::Serial1 serial1;
    Example::Serial1 serial2;

    // Check serializable default values are correctly set
    ASSERT_EQ(serial1.getMember1(), 12345);
    ASSERT_EQ(serial1.getMember2(), "hello");
    ASSERT_EQ(serial1.getMember3(), Example::MEM2);

    // Check serializable scalar default for array member is correctly set
    NATIVE_INT_TYPE size;
    const U32* serial1Member4 = serial1.getMember4(size);
    for (NATIVE_INT_TYPE _mem = 0; _mem < size; _mem++) {
        ASSERT_EQ(serial1Member4[_mem], 3);
    }
}

TEST(Serialize4, ArrayScalarInit) {

    Example::Serial1 serial1 (0,"hello world",Example::SomeEnum::MEM2,2);

    // Check serializable array member values are correctly set
    NATIVE_INT_TYPE size;
    const U32* serialMember4 = serial1.getMember4(size);
    for (NATIVE_INT_TYPE _mem = 0; _mem < size; _mem++) {
        ASSERT_EQ(serialMember4[_mem], 2);
    }
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    int status = RUN_ALL_TESTS();

    cout << "Completed..." << endl;

    return status;
}
