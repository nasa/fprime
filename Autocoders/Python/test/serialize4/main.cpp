#include <Autocoders/Python/test/serialize4/Serial1SerializableAc.hpp>

#include "gtest/gtest.h"

using namespace std;

TEST(DefaultValues, OK) {

    Example::Serial1 serial1;

    // Check serializable default values are correctly set
    ASSERT_EQ(serial1.getMember1(), 12345);
    ASSERT_EQ(serial1.getMember2(), "hello");
    ASSERT_EQ(serial1.getMember3(), Example::MEM2);

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    int status = RUN_ALL_TESTS();

    cout << "Completed..." << endl;

    return status;
}
