// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"

TEST(Nominal, MetaDataPdu) {
    // Allocate tester on heap to avoid stack overflow (CfdpManager is very large)
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testMetaDataPdu();
    delete tester;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
