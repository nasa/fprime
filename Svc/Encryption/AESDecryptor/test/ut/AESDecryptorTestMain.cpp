// ======================================================================
// \title  AESDecryptorTestMain.cpp
// \author vivi
// \brief  cpp file for AESDecryptor component test main function
// ======================================================================

#include "AESDecryptorTester.hpp"

TEST(Nominal, toDo) {
    Svc::Ccsds::AESDecryptorTester tester;
    tester.toDo();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
