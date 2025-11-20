// ======================================================================
// \title  ByteStreamBufferAdapterTestMain.cpp
// \author shahab
// \brief  cpp file for ByteStreamBufferAdapter component test main function
// ======================================================================

#include "ByteStreamBufferAdapterTester.hpp"

TEST(Nominal, toDo) {
    Drv::ByteStreamBufferAdapterTester tester;
    tester.toDo();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
