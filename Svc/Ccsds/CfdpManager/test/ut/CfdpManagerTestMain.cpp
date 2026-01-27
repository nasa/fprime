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

TEST(Transaction, Class1TxNominal) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass1TxNominal();
    delete tester;
}

TEST(Transaction, Class2TxNominal) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass2TxNominal();
    delete tester;
}

TEST(Transaction, Class2TxNack) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass2TxNack();
    delete tester;
}

TEST(Transaction, Class1RxNominal) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass1RxNominal();
    delete tester;
}

TEST(Transaction, Class2RxNominal) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass2RxNominal();
    delete tester;
}

TEST(Transaction, Class2RxNack) {
    Svc::Ccsds::CfdpManagerTester* tester = new Svc::Ccsds::CfdpManagerTester();
    tester->testClass2RxNack();
    delete tester;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
