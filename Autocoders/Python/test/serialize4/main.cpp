#include <Autocoders/Python/test/serialize4/Serial1SerializableAc.hpp>

#include "gtest/gtest.h"

using namespace std;

TEST(StringArray, OK) {

    Example::Serial1 serial1;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    int status = RUN_ALL_TESTS();

    cout << "Completed..." << endl;

    return status;
}