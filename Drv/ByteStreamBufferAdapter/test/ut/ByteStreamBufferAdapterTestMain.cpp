// ======================================================================
// \title  ByteStreamBufferAdapterTestMain.cpp
// \author shahab
// \brief  cpp file for ByteStreamBufferAdapter component test main function
// ======================================================================

#include "ByteStreamBufferAdapterTester.hpp"

TEST(Nominal, Test_Stream_out) {
    Drv::ByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_out();
}

TEST(Nominal, Test_Stream_in) {
    Drv::ByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_in();
}

TEST(Nominal, Test_Stream_Return) {
    Drv::ByteStreamBufferAdapterTester tester;
    tester.test_byte_stream_return();
}

TEST(Nominal, Test_Driver_Ready_State) {
    Drv::ByteStreamBufferAdapterTester tester;
    tester.test_driver_ready_state();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
