// ======================================================================
// \title  AsyncByteStreamBufferAdapterTestMain.cpp
// \author shahab
// \brief  cpp file for AsyncByteStreamBufferAdapter component test main function
// ======================================================================

#include "AsyncByteStreamBufferAdapterTester.hpp"

TEST(Nominal, Test_Stream_out) {
    Drv::AsyncByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_out();
}

TEST(Nominal, Test_Stream_in) {
    Drv::AsyncByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_in();
}
TEST(Nominal, Test_Stream_Return) {
    Drv::AsyncByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_return();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
