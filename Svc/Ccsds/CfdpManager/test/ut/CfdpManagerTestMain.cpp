// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"

TEST(Nominal, MetaDataPdu) {
    Svc::Ccsds::CfdpManagerTester tester;
    tester.testMetaDataPdu();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
