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

TEST(Transaction, Class2TxPortBased) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2TxPortBased();
    delete tester;
}

TEST(Transaction, Class2TxPortBasedNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2TxPortBasedNack();
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

TEST(Parameter, LocalEidSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamLocalEidSetGet();
    delete tester;
}

TEST(Parameter, LocalEidDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamLocalEidDefault();
    delete tester;
}

TEST(Parameter, OutgoingFileChunkSizeSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamOutgoingFileChunkSizeSetGet();
    delete tester;
}

TEST(Parameter, OutgoingFileChunkSizeDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamOutgoingFileChunkSizeDefault();
    delete tester;
}

TEST(Parameter, RxCrcCalcBytesPerCycleSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamRxCrcCalcBytesPerCycleSetGet();
    delete tester;
}

TEST(Parameter, RxCrcCalcBytesPerCycleDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamRxCrcCalcBytesPerCycleDefault();
    delete tester;
}

TEST(Parameter, FileInDefaultChannelSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultChannelSetGet();
    delete tester;
}

TEST(Parameter, FileInDefaultChannelDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultChannelDefault();
    delete tester;
}

TEST(Parameter, FileInDefaultDestEntityIdSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultDestEntityIdSetGet();
    delete tester;
}

TEST(Parameter, FileInDefaultDestEntityIdDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultDestEntityIdDefault();
    delete tester;
}

TEST(Parameter, FileInDefaultClassSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultClassSetGet();
    delete tester;
}

TEST(Parameter, FileInDefaultClassDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultClassDefault();
    delete tester;
}

TEST(Parameter, FileInDefaultKeepSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultKeepSetGet();
    delete tester;
}

TEST(Parameter, FileInDefaultKeepDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultKeepDefault();
    delete tester;
}

TEST(Parameter, FileInDefaultPrioritySetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultPrioritySetGet();
    delete tester;
}

TEST(Parameter, FileInDefaultPriorityDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testParamFileInDefaultPriorityDefault();
    delete tester;
}

TEST(Port, DataReturnInChannel0) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testDataReturnInChannel0();
    delete tester;
}

TEST(Port, DataReturnInChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testDataReturnInChannel1();
    delete tester;
}

TEST(MultiChannel, Class1RxNominalChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass1RxNominalChannel1();
    delete tester;
}

TEST(MultiChannel, Class2RxNominalChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testClass2RxNominalChannel1();
    delete tester;
}

TEST(Event, TxFileQueued) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxFileQueuedEvent();
    delete tester;
}

TEST(Event, TxFileTransferStarted) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxFileTransferStartedEvent();
    delete tester;
}

TEST(Event, MetadataReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testMetadataReceivedEvent();
    delete tester;
}

TEST(Event, BuffersExhausted) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testBuffersExhaustedEvent();
    delete tester;
}

TEST(Event, FailPduHeaderDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailPduHeaderDeserializationEvent();
    delete tester;
}

TEST(Event, FailMetadataPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailMetadataPduDeserializationEvent();
    delete tester;
}

TEST(Event, RxFileCreateFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxFileCreateFailedEvent();
    delete tester;
}

TEST(Event, RxCrcMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxCrcMismatchEvent();
    delete tester;
}

TEST(Event, RxFileSizeMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxFileSizeMismatchEvent();
    delete tester;
}

TEST(Event, RxEofCancelReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxEofCancelReceivedEvent();
    delete tester;
}

TEST(Event, RxTransactionLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxTransactionLimitReachedEvent();
    delete tester;
}

TEST(Event, InvalidDestinationEid) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testInvalidDestinationEidEvent();
    delete tester;
}

TEST(Event, MaxTxTransactionsReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testMaxTxTransactionsReachedEvent();
    delete tester;
}

TEST(Event, FileRemoveFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFileRemoveFailedEvent();
    delete tester;
}

TEST(Event, PlaybackDirOpenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirOpenFailedEvent();
    delete tester;
}

TEST(Event, FailFileDataPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailFileDataPduDeserializationEvent();
    delete tester;
}

TEST(Event, FailEofPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailEofPduDeserializationEvent();
    delete tester;
}

TEST(Event, FailAckPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailAckPduDeserializationEvent();
    delete tester;
}

TEST(Event, FailFinPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailFinPduDeserializationEvent();
    delete tester;
}

TEST(Event, FailNakPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailNakPduDeserializationEvent();
    delete tester;
}

// Category A: New RX Error Tests
TEST(Event, RxEofWithError) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxEofWithErrorEvent();
    delete tester;
}

TEST(Event, RxEofMdSizeMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxEofMdSizeMismatchEvent();
    delete tester;
}

TEST(Event, RxInvalidDirectiveCode) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxInvalidDirectiveCodeEvent();
    delete tester;
}

TEST(Event, RxInactivityTimeout) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxInactivityTimeoutEvent();
    delete tester;
}

TEST(Event, RxAckLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxAckLimitReachedEvent();
    delete tester;
}

TEST(Event, RxNakLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxNakLimitReachedEvent();
    delete tester;
}

TEST(Event, UnhandledPduInIdleState) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testUnhandledPduInIdleStateEvent();
    delete tester;
}

TEST(Event, TxInvalidDirectiveCode) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxInvalidDirectiveCodeEvent();
    delete tester;
}

TEST(Event, TxEarlyFinReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxEarlyFinReceivedEvent();
    delete tester;
}

TEST(Event, TxNonFileDirectivePduReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxNonFileDirectivePduReceivedEvent();
    delete tester;
}

TEST(Event, TxInvalidNakPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxInvalidNakPduEvent();
    delete tester;
}

TEST(Event, TxInvalidSegmentRequests) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxInvalidSegmentRequestsEvent();
    delete tester;
}

TEST(Event, TxInactivityTimeout) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxInactivityTimeoutEvent();
    delete tester;
}

TEST(Event, TxAckLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxAckLimitReachedEvent();
    delete tester;
}

TEST(Event, RxWriteFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxWriteFailedEvent();
    delete tester;
}

TEST(Event, RxSeekFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxSeekFailedEvent();
    delete tester;
}

TEST(Event, RxFileRenameFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxFileRenameFailedEvent();
    delete tester;
}

TEST(Event, TxFileSeekFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxFileSeekFailedEvent();
    delete tester;
}

TEST(Event, TxSendMetadataFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testTxSendMetadataFailedEvent();
    delete tester;
}

TEST(Event, RxTempFileCreated) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxTempFileCreatedEvent();
    delete tester;
}

TEST(Event, DanglingFileHandleClosed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testDanglingFileHandleClosedEvent();
    delete tester;
}

TEST(Event, ResetFreedTransaction) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testResetFreedTransactionEvent();
    delete tester;
}

TEST(Event, RxSeekCrcFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxSeekCrcFailedEvent();
    delete tester;
}

TEST(Event, RxReadCrcFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxReadCrcFailedEvent();
    delete tester;
}

TEST(Event, UnsupportedSendFileArguments) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testUnsupportedSendFileArgumentsEvent();
    delete tester;
}

TEST(Event, SendFileInitiateFail) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testSendFileInitiateFailEvent();
    delete tester;
}

TEST(Event, InvalidChannelPoll) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testInvalidChannelPollEvent();
    delete tester;
}

TEST(Event, ChunklistUnavailable) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testChunklistUnavailableEvent();
    delete tester;
}

TEST(Event, FailKeepFileMove) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailKeepFileMoveEvent();
    delete tester;
}

TEST(Event, FailPduSerialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailPduSerializationEvent();
    delete tester;
}

TEST(Event, FailPollFileMove) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFailPollFileMoveEvent();
    delete tester;
}

TEST(Event, FileDataSegmentMetadata) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFileDataSegmentMetadataEvent();
    delete tester;
}

TEST(Event, PlaybackDirReadFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirReadFailedEvent();
    delete tester;
}

TEST(Event, PlaybackDirSlotUnavailable) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPlaybackDirSlotUnavailableEvent();
    delete tester;
}

TEST(Event, RxFileReopenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testRxFileReopenFailedEvent();
    delete tester;
}

TEST(Miscellaneous, Ping) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPing();
    delete tester;
}

// ----------------------------------------------------------------------
// Coverage Tests
// ----------------------------------------------------------------------

TEST(Coverage, DataInBufferTooSmall) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testDataInBufferTooSmall();
    delete tester;
}

TEST(Coverage, DataInWrongDescriptor) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testDataInWrongDescriptor();
    delete tester;
}

TEST(Coverage, GetPduBufferMaxOutgoing) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testGetPduBufferMaxOutgoing();
    delete tester;
}

TEST(Coverage, CancelTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testCancelTransactionNotFound();
    delete tester;
}

TEST(Coverage, AbandonTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testAbandonTransactionNotFound();
    delete tester;
}

TEST(Coverage, ResetCountersInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testResetCountersInvalidChannel();
    delete tester;
}

TEST(Coverage, IncrementRecvDropped) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testIncrementRecvDropped();
    delete tester;
}

TEST(Coverage, IncrementSentEofCanceled) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testIncrementSentEofCanceled();
    delete tester;
}

TEST(Coverage, GetTxnStatusStates) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testGetTxnStatusStates();
    delete tester;
}

TEST(Coverage, FindBySequenceNumberCallback) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testFindBySequenceNumberCallback();
    delete tester;
}

TEST(Coverage, PrioritySearchCallback) {
    Svc::Ccsds::Cfdp::CfdpManagerTester* tester = new Svc::Ccsds::Cfdp::CfdpManagerTester();
    tester->testPrioritySearchCallback();
    delete tester;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
