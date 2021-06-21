#include <Autocoders/Python/test/serialize4/Serial1SerializableAc.hpp>

#include "gtest/gtest.h"

using namespace std;

TEST(Serialize4, ArrayScalarInit) {

    Example::Serial1 serial1 (0,"hello world",Example::SomeEnum::MEM2,2);


    //Check serializable array member values are correctly set
    NATIVE_INT_TYPE size;
    const U32* serialMember4 = serial1.getMember4(size);
    for (NATIVE_INT_TYPE _mem = 0; _mem < size; _mem++) {
        ASSERT_EQ(serialMember4[0], 2);
    }
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    int status = RUN_ALL_TESTS();

    cout << "Completed..." << endl;

    return status;
}
