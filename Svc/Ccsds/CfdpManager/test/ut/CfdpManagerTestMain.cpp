// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"

TEST(Pdu, MetaDataPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testMetaDataPdu();
    delete tester;
}

TEST(Pdu, FileDataPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testFileDataPdu();
    delete tester;
}

TEST(Pdu, EofPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testEofPdu();
    delete tester;
}

TEST(Pdu, FinPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testFinPdu();
    delete tester;
}

TEST(Pdu, AckPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testAckPdu();
    delete tester;
}

TEST(Pdu, NakPdu) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testNakPdu();
    delete tester;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
