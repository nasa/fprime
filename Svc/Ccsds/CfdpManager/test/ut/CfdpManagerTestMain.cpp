// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"

TEST(Pdu, MetaDataPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testMetaDataPdu();
    delete tester;
}

TEST(Pdu, FileDataPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFileDataPdu();
    delete tester;
}

TEST(Pdu, EofPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testEofPdu();
    delete tester;
}

TEST(Pdu, FinPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFinPdu();
    delete tester;
}

TEST(Pdu, AckPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testAckPdu();
    delete tester;
}

TEST(Pdu, NakPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testNakPdu();
    delete tester;
}

TEST(Transaction, Class1TxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass1TxNominal();
    delete tester;
}

TEST(Transaction, Class2TxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2TxNominal();
    delete tester;
}

TEST(Transaction, Class2TxNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2TxNack();
    delete tester;
}

TEST(Transaction, Class1RxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass1RxNominal();
    delete tester;
}

TEST(Transaction, Class2RxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2RxNominal();
    delete tester;
}

TEST(Transaction, Class2RxNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2RxNack();
    delete tester;
}

TEST(Transaction, Class1TxPortBased) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2TxPortBased();
    delete tester;
}

TEST(Transaction, MultipleTransactionsInSeries) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testMultipleTransactionsInSeries();
    delete tester;
}

TEST(Command, SendFileZeroLength) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSendFileZeroLength();
    delete tester;
}

TEST(Command, SendFileNonExistent) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSendFileNonExistent();
    delete tester;
}

TEST(Command, StopPollDirNotActive) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testStopPollDirNotActive();
    delete tester;
}

TEST(Command, StopPollDirActive) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testStopPollDirActive();
    delete tester;
}

TEST(Command, SendFileInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSendFileInvalidChannel();
    delete tester;
}

TEST(Command, PlaybackDirectoryNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirectoryNominal();
    delete tester;
}

TEST(Command, PlaybackDirectoryInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirectoryInvalidChannel();
    delete tester;
}

TEST(Command, PlaybackDirectoryOpenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirectoryOpenFailed();
    delete tester;
}

TEST(Command, PollDirectoryNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPollDirectoryNominal();
    delete tester;
}

TEST(Command, PollDirectoryInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPollDirectoryInvalidChannel();
    delete tester;
}

TEST(Command, PollDirectoryBusy) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPollDirectoryBusy();
    delete tester;
}

TEST(Command, SetChannelFlowNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSetChannelFlowNominal();
    delete tester;
}

TEST(Command, SetChannelFlowInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSetChannelFlowInvalidChannel();
    delete tester;
}

TEST(Command, SuspendResumeTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSuspendResumeTransactionNominal();
    delete tester;
}

TEST(Command, SuspendResumeTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSuspendResumeTransactionInvalidChannel();
    delete tester;
}

TEST(Command, SuspendResumeTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSuspendResumeTransactionNotFound();
    delete tester;
}

TEST(Command, CancelTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testCancelTransactionNominal();
    delete tester;
}

TEST(Command, CancelTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testCancelTransactionInvalidChannel();
    delete tester;
}

TEST(Command, AbandonTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testAbandonTransactionNominal();
    delete tester;
}

TEST(Command, AbandonTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testAbandonTransactionInvalidChannel();
    delete tester;
}

TEST(Command, ResetCountersSingleChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testResetCountersSingleChannel();
    delete tester;
}

TEST(Command, ResetCountersAllChannels) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testResetCountersAllChannels();
    delete tester;
}

TEST(Miscellaneous, Ping) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPing();
    delete tester;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
