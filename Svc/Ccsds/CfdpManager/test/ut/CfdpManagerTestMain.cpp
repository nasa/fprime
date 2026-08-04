// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"

TEST(Pdu, MetaDataPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testMetaDataPdu();
}

TEST(Pdu, FileDataPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFileDataPdu();
}

TEST(Pdu, EofPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testEofPdu();
}

TEST(Pdu, FinPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFinPdu();
}

TEST(Pdu, AckPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAckPdu();
}

TEST(Pdu, NakPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testNakPdu();
}

TEST(Transaction, Class1TxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1TxNominal();
}

TEST(Transaction, Class2TxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxNominal();
}

TEST(Transaction, Class2TxNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxNack();
}

TEST(Transaction, Class2TxLateFinAck) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxLateFinAck();
}

TEST(Transaction, Class1RxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1RxNominal();
}

TEST(Transaction, Class2RxNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxNominal();
}

TEST(Transaction, Class2RxNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxNack();
}

TEST(Transaction, Class2TxPortBased) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxPortBased();
}

TEST(Transaction, Class2TxPortBasedNack) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxPortBasedNack();
}

TEST(Transaction, MultipleTransactionsInSeries) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testMultipleTransactionsInSeries();
}

TEST(Command, SendFileZeroLength) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSendFileZeroLength();
}

TEST(Command, SendFileNonExistent) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSendFileNonExistent();
}

TEST(Command, StopPollDirNotActive) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testStopPollDirNotActive();
}

TEST(Command, StopPollDirActive) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testStopPollDirActive();
}

TEST(Command, SendFileInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSendFileInvalidChannel();
}

TEST(Command, PlaybackDirectoryNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirectoryNominal();
}

TEST(Command, PlaybackDirectoryInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirectoryInvalidChannel();
}

TEST(Command, PlaybackDirectoryOpenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirectoryOpenFailed();
}

TEST(Command, PollDirectoryNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPollDirectoryNominal();
}

TEST(Command, PollDirectoryInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPollDirectoryInvalidChannel();
}

TEST(Command, PollDirectoryBusy) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPollDirectoryBusy();
}

TEST(Command, SetChannelFlowNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSetChannelFlowNominal();
}

TEST(Command, SetChannelFlowInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSetChannelFlowInvalidChannel();
}

TEST(Command, SuspendResumeTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSuspendResumeTransactionNominal();
}

TEST(Command, SuspendResumeTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSuspendResumeTransactionInvalidChannel();
}

TEST(Command, SuspendResumeTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSuspendResumeTransactionNotFound();
}

TEST(Command, CancelTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testCancelTransactionNominal();
}

TEST(Command, CancelTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testCancelTransactionInvalidChannel();
}

TEST(Command, AbandonTransactionNominal) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonTransactionNominal();
}

TEST(Command, AbandonTransactionInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonTransactionInvalidChannel();
}

TEST(Command, ResetCountersSingleChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testResetCountersSingleChannel();
}

TEST(Command, ResetCountersAllChannels) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testResetCountersAllChannels();
}

TEST(Parameter, LocalEidSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamLocalEidSetGet();
}

TEST(Parameter, LocalEidDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamLocalEidDefault();
}

TEST(Parameter, OutgoingFileChunkSizeSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamOutgoingFileChunkSizeSetGet();
}

TEST(Parameter, OutgoingFileChunkSizeDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamOutgoingFileChunkSizeDefault();
}

TEST(Parameter, RxCrcCalcBytesPerCycleSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamRxCrcCalcBytesPerCycleSetGet();
}

TEST(Parameter, RxCrcCalcBytesPerCycleDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamRxCrcCalcBytesPerCycleDefault();
}

TEST(Parameter, FileInDefaultChannelSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultChannelSetGet();
}

TEST(Parameter, FileInDefaultChannelDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultChannelDefault();
}

TEST(Parameter, FileInDefaultDestEntityIdSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultDestEntityIdSetGet();
}

TEST(Parameter, FileInDefaultDestEntityIdDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultDestEntityIdDefault();
}

TEST(Parameter, FileInDefaultClassSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultClassSetGet();
}

TEST(Parameter, FileInDefaultClassDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultClassDefault();
}

TEST(Parameter, FileInDefaultKeepSetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultKeepSetGet();
}

TEST(Parameter, FileInDefaultKeepDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultKeepDefault();
}

TEST(Parameter, FileInDefaultPrioritySetGet) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultPrioritySetGet();
}

TEST(Parameter, FileInDefaultPriorityDefault) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamFileInDefaultPriorityDefault();
}

TEST(Port, DataReturnInChannel0) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testDataReturnInChannel0();
}

TEST(Port, DataReturnInChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testDataReturnInChannel1();
}

TEST(MultiChannel, Class1RxNominalChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1RxNominalChannel1();
}

TEST(MultiChannel, Class2RxNominalChannel1) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxNominalChannel1();
}

TEST(Event, TxFileQueued) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxFileQueuedEvent();
}

TEST(Event, TxFileTransferStarted) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxFileTransferStartedEvent();
}

TEST(Event, MetadataReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testMetadataReceivedEvent();
}

TEST(Event, BuffersExhausted) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testBuffersExhaustedEvent();
}

TEST(Event, FailPduHeaderDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailPduHeaderDeserializationEvent();
}

TEST(Event, FailMetadataPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailMetadataPduDeserializationEvent();
}

TEST(Event, RxFileCreateFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileCreateFailedEvent();
}

TEST(Event, RxCrcMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxCrcMismatchEvent();
}

TEST(Event, RxFileSizeMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileSizeMismatchEvent();
}

TEST(Event, RxFileDataOutOfBounds) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileDataOutOfBoundsEvent();
}

TEST(Event, RxEofCancelReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxEofCancelReceivedEvent();
}

TEST(Event, RxTransactionLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxTransactionLimitReachedEvent();
}

TEST(Event, InvalidDestinationEid) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testInvalidDestinationEidEvent();
}

TEST(Event, MaxTxTransactionsReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testMaxTxTransactionsReachedEvent();
}

TEST(Event, FileRemoveFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFileRemoveFailedEvent();
}

TEST(Event, PlaybackDirOpenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirOpenFailedEvent();
}

TEST(Event, FailFileDataPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailFileDataPduDeserializationEvent();
}

TEST(Event, FailEofPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailEofPduDeserializationEvent();
}

TEST(Event, FailAckPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailAckPduDeserializationEvent();
}

TEST(Event, FailFinPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailFinPduDeserializationEvent();
}

TEST(Event, FailNakPduDeserialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailNakPduDeserializationEvent();
}

// Category A: New RX Error Tests
TEST(Event, RxEofWithError) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxEofWithErrorEvent();
}

TEST(Event, RxEofMdSizeMismatch) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxEofMdSizeMismatchEvent();
}

TEST(Event, RxInvalidDirectiveCode) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxInvalidDirectiveCodeEvent();
}

TEST(Event, RxInactivityTimeout) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxInactivityTimeoutEvent();
}

TEST(Event, RxAckLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxAckLimitReachedEvent();
}

TEST(Event, RxNakLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxNakLimitReachedEvent();
}

TEST(Event, UnhandledPduInIdleState) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testUnhandledPduInIdleStateEvent();
}

TEST(Event, TxInvalidDirectiveCode) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxInvalidDirectiveCodeEvent();
}

TEST(Event, TxEarlyFinReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxEarlyFinReceivedEvent();
}

TEST(Event, TxNonFileDirectivePduReceived) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxNonFileDirectivePduReceivedEvent();
}

TEST(Event, TxInvalidNakPdu) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxInvalidNakPduEvent();
}

TEST(Event, TxInvalidSegmentRequests) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxInvalidSegmentRequestsEvent();
}

TEST(Event, TxInactivityTimeout) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxInactivityTimeoutEvent();
}

TEST(Event, TxAckLimitReached) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxAckLimitReachedEvent();
}

TEST(Event, RxWriteFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxWriteFailedEvent();
}

TEST(Event, RxSeekFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxSeekFailedEvent();
}

TEST(Event, RxFileRenameFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileRenameFailedEvent();
}

TEST(Event, TxFileSeekFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxFileSeekFailedEvent();
}

TEST(Event, TxSendMetadataFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxSendMetadataFailedEvent();
}

TEST(Event, RxTempFileCreated) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxTempFileCreatedEvent();
}

TEST(Event, DanglingFileHandleClosed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testDanglingFileHandleClosedEvent();
}

TEST(Event, ResetFreedTransaction) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testResetFreedTransactionEvent();
}

TEST(Event, RxSeekCrcFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxSeekCrcFailedEvent();
}

TEST(Event, RxReadCrcFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxReadCrcFailedEvent();
}

TEST(Event, UnsupportedSendFileArguments) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testUnsupportedSendFileArgumentsEvent();
}

TEST(Event, SendFileInitiateFail) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testSendFileInitiateFailEvent();
}

TEST(Event, InvalidChannelPoll) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testInvalidChannelPollEvent();
}

TEST(Event, ChunklistUnavailable) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testChunklistUnavailableEvent();
}

TEST(Event, FailKeepFileMove) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailKeepFileMoveEvent();
}

TEST(Event, FailPduSerialization) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailPduSerializationEvent();
}

TEST(Event, FailPollFileMove) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailPollFileMoveEvent();
}

TEST(Event, FileDataSegmentMetadata) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFileDataSegmentMetadataEvent();
}

TEST(Event, PlaybackDirReadFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirReadFailedEvent();
}

TEST(Event, PlaybackDirSlotUnavailable) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirSlotUnavailableEvent();
}

TEST(Event, RxFileReopenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileReopenFailedEvent();
}

TEST(Miscellaneous, Ping) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPing();
}

// ----------------------------------------------------------------------
// Coverage Tests
// ----------------------------------------------------------------------

TEST(Coverage, DataInBufferTooSmall) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testDataInBufferTooSmall();
}

TEST(Coverage, DataInWrongDescriptor) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testDataInWrongDescriptor();
}

TEST(Coverage, GetPduBufferMaxOutgoing) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testGetPduBufferMaxOutgoing();
}

TEST(Coverage, CancelTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testCancelTransactionNotFound();
}

TEST(Coverage, AbandonTransactionNotFound) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonTransactionNotFound();
}

TEST(Coverage, ResetCountersInvalidChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testResetCountersInvalidChannel();
}

TEST(Coverage, IncrementRecvDropped) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testIncrementRecvDropped();
}

TEST(Coverage, IncrementSentEofCanceled) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testIncrementSentEofCanceled();
}

TEST(Coverage, GetTxnStatusStates) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testGetTxnStatusStates();
}

TEST(Coverage, FindBySequenceNumberCallback) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFindBySequenceNumberCallback();
}

TEST(Coverage, PrioritySearchCallback) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPrioritySearchCallback();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
