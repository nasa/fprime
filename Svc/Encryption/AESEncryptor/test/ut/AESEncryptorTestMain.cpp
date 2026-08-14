// ======================================================================
// \title  AESEncryptorTestMain.cpp
// \author vivi
// \brief  cpp file for AESEncryptor component test main function
// ======================================================================

#include "AESEncryptorTester.hpp"

TEST(Nominal, toDo) {
    Svc::Ccsds::AESEncryptorTester tester;
    tester.toDo();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
