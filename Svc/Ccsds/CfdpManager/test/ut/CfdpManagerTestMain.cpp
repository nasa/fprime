// ======================================================================
// \title  CfdpManagerTestMain.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component test main function
// ======================================================================

#include "CfdpManagerTester.hpp"
#include "Fw/Test/UnitTest.hpp"

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

TEST(Pdu, NakPduSegmentCountLimit) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testNakPduSegmentCountLimit();
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

TEST(Transaction, Class2TxQueuedEarlyFinNoHang) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxQueuedEarlyFinNoHang();
}

TEST(Transaction, Class2TxQueuedAbandonNoHang) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxQueuedAbandonNoHang();
}

TEST(Transaction, Class2TxMalformedEarlyFinNoSpin) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxMalformedEarlyFinNoSpin();
}

TEST(Transaction, Class2TxThrottledNakMetadataYields) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxThrottledNakMetadataYields();
}

TEST(Transaction, Class2TxThrottledNakChunkRetries) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxThrottledNakChunkRetries();
}

TEST(Transaction, Class2TxThrottledInitialMetadataYields) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2TxThrottledInitialMetadataYields();
}

TEST(Transaction, PlaybackThrottledMetadataYields) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackThrottledMetadataYields();
}

TEST(Transaction, TxCycleNoProgressFailsTransaction) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTxCycleNoProgressFailsTransaction();
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

TEST(Transaction, Class2RxZeroLengthFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxZeroLengthFile();
}

TEST(Transaction, Class2RxFileDataOffsetOverflow) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxFileDataOffsetOverflow();
}

TEST(Transaction, Class2RxTruncatedFileDataCrcSpin) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxTruncatedFileDataCrcSpin();
}

TEST(Transaction, Class2RxCrcShortFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxCrcShortFile();
}

// Regression test for GHSA-mh5x-2m6h-8267: a zero-length FileData segment must not assert/FATAL
TEST(Transaction, Class2RxZeroLengthFileData) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxZeroLengthFileData();
}

TEST(Transaction, Class1RxChecksumMismatchRemovesFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1RxChecksumMismatchRemovesFile();
}

TEST(Transaction, Class1RxSizeMismatchRemovesFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1RxSizeMismatchRemovesFile();
}

TEST(Transaction, Class2RxChecksumMismatchRemovesFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxChecksumMismatchRemovesFile();
}

TEST(Transaction, Class1RxInactivityRemovesFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass1RxInactivityRemovesFile();
}

TEST(Transaction, Class2RxStalledRemovesFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxStalledRemovesFile();
}

TEST(Transaction, Class2RxWithoutMetadataRemovesTempFile) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testClass2RxWithoutMetadataRemovesTempFile();
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

TEST(Command, PollDirectoryInvalidInterval) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPollDirectoryInvalidInterval();
}

TEST(Command, PollDirectoryTimerExpiry) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPollDirectoryTimerExpiry();
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

TEST(Command, AbandonTransactionTwice) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonTransactionTwice();
}

TEST(Command, AbandonTransactionAfterCompletion) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonTransactionAfterCompletion();
}

TEST(Command, AbandonPlaybackTransactionAfterCompletion) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testAbandonPlaybackTransactionAfterCompletion();
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

TEST(ParameterTelemetry, LocalEid) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmLocalEid();
}

TEST(ParameterTelemetry, OutgoingFileChunkSize) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmOutgoingFileChunkSize();
}

TEST(ParameterTelemetry, RxCrcCalcBytesPerCycle) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmRxCrcCalcBytesPerCycle();
}

TEST(ParameterTelemetry, PostInactivitySendRetries) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmPostInactivitySendRetries();
}

TEST(ParameterTelemetry, FileInDefaultChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmFileInDefaultChannel();
}

TEST(ParameterTelemetry, FileInDefaultDestEntityId) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmFileInDefaultDestEntityId();
}

TEST(ParameterTelemetry, FileInDefaultClass) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmFileInDefaultClass();
}

TEST(ParameterTelemetry, FileInDefaultKeep) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmFileInDefaultKeep();
}

TEST(ParameterTelemetry, FileInDefaultPriority) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmFileInDefaultPriority();
}

TEST(ParameterTelemetry, ChannelConfig) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTlmChannelConfig();
}

TEST(ParameterTelemetry, UnknownIdAsserts) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParameterUpdatedUnknownIdAsserts();
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

TEST(Event, FailPduHeaderDeserializationUnsupportedVersion) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testFailPduHeaderDeserializationUnsupportedVersionEvent();
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

TEST(Event, OrphanPdusDoNotHoldSlots) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testOrphanPdusDoNotHoldSlots();
}

TEST(Event, RxCapReservesTxCapacity) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxCapReservesTxCapacity();
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

TEST(Event, PlaybackDirEntrySourcePathTooLong) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirEntrySourcePathTooLong();
}

TEST(Event, PlaybackDirEntryDestinationPathTooLong) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPlaybackDirEntryDestinationPathTooLong();
}

TEST(Timer, ZeroDurationExpiresOnFirstTick) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testTimerZeroDurationExpiresOnFirstTick();
}

TEST(Timer, ZeroInactivityTimerExpiresOnFirstTick) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testZeroInactivityTimerExpiresOnFirstTick();
}

TEST(Event, RxFileReopenFailed) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxFileReopenFailedEvent();
}

TEST(Event, RxDestPathRejected) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxDestPathRejectedEvent();
}

TEST(Event, RxDestPathContained) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxDestPathContained();
}

TEST(Event, RxDestPathRejectedLateMetadata) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxDestPathRejectedLateMetadata();
}

TEST(Event, RxDirPerChannel) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxDirPerChannel();
}

TEST(Event, RxDirUnresolvable) {
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxDirUnresolvable();
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

TEST(RxStaging, Class1TimeoutKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-009");
    COMMENT("A Class 1 receive cut off by inactivity leaves an existing destination file unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1TimeoutKeepsExistingFile();
}

TEST(RxStaging, Class2NakLimitKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-008");
    COMMENT("A Class 2 receive ended by its NAK and ACK limits leaves an existing destination file unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2NakLimitKeepsExistingFile();
}

TEST(RxStaging, Class1CommitReplacesExistingFile) {
    REQUIREMENT("CFDP-016");
    COMMENT("A verified Class 1 receive replaces the destination only at EOF, leaving no staging file.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1CommitReplacesExistingFile();
}

TEST(RxStaging, Class1CrcMismatchKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-004");
    COMMENT("A Class 1 checksum mismatch is reported as failed and leaves the destination unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1CrcMismatchKeepsExistingFile();
}

TEST(RxStaging, Class2CrcMismatchKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-004");
    COMMENT("A Class 2 checksum mismatch is reported as failed and leaves the destination unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2CrcMismatchKeepsExistingFile();
}

TEST(RxStaging, Class1EofCancelKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    COMMENT("A Class 1 receive cancelled by its EOF leaves the destination unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1EofCancelKeepsExistingFile();
}

TEST(RxStaging, Class2LateMetadataCommits) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-002");
    COMMENT("File Data received before Metadata is staged and committed only after verification.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataCommits();
}

TEST(RxStaging, Class2LateMetadataMoveFailureKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    COMMENT("A failed move of the temp file removes the claimed staging file; repeated Metadata is ignored.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataMoveFailureKeepsExistingFile();
}

TEST(RxStaging, Class2LateMetadataOversizeDataNotCommitted) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "Class 2 File Data received before Metadata that extends past the declared size fails the transfer with "
        "FILE_SIZE_ERROR; the destination is unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataOversizeDataNotCommitted();
}

TEST(RxStaging, Class2LateMetadataUnknownTempSizeFails) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "If the temp file's size cannot be read when late Metadata arrives, the transfer fails with FILE_SIZE_ERROR "
        "before anything is staged; the destination is unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataUnknownTempSizeFails();
}

TEST(RxStaging, Class2RepeatedMetadataIgnored) {
    REQUIREMENT("CFDP-016");
    COMMENT("A Metadata PDU repeated after the first was accepted claims and moves no further file.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2RepeatedMetadataIgnored();
}

TEST(RxStaging, Class2ExistingStagingFileNotReplaced) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A Class 2 receive with Metadata first does not replace an existing file at its staging path; it fails with "
        "FILESTORE_REJECTION and the FIN does not report the file as retained.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2ExistingStagingFileNotReplaced();
}

TEST(RxStaging, Class1EofSizeMismatchFails) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A Class 1 EOF whose size differs from the Metadata size fails with FILE_SIZE_ERROR; nothing is committed.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1EofSizeMismatchFails();
}

TEST(RxStaging, NotKeptReceiveRemovesOnlyOwnedFile) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "Disposing of a receive that is not kept removes only the staging file it created; the existing destination "
        "is unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingNotKeptReceiveRemovesOnlyOwnedFile();
}

TEST(RxStaging, Class1FlushFailureNotCommitted) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "Data that cannot be flushed to stable storage before the rename is not committed; the transfer fails with "
        "FILESTORE_REJECTION and the existing destination is unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1FlushFailureNotCommitted();
}

TEST(RxStaging, Class2PausedChecksumPassCommits) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A Class 2 checksum pass paused by RxCrcCalcBytesPerCycle = 0 does not fail the transfer; it commits later.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2PausedChecksumPassCommits();
}

TEST(RxStaging, Class2LateMetadataSizeMismatchThenRetransmit) {
    REQUIREMENT("CFDP-016");
    COMMENT("After an EOF/Metadata size mismatch, repeated Metadata and File Data never reach the destination.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataSizeMismatchThenRetransmit();
}

TEST(RxStaging, ExistingStagingFileNotReplaced) {
    REQUIREMENT("CFDP-016");
    COMMENT("An existing file at the staging path is neither replaced nor removed; the transfer fails.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingExistingStagingFileNotReplaced();
}

TEST(RxStaging, Class2LateMetadataExistingStagingFileNotReplaced) {
    REQUIREMENT("CFDP-016");
    COMMENT("An existing file at the staging path is not replaced when Metadata arrives late.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2LateMetadataExistingStagingFileNotReplaced();
}

TEST(RxStaging, CommitFailureKeepsDestination) {
    REQUIREMENT("CFDP-016");
    COMMENT("A failed Class 1 rename onto the destination keeps it and removes the staging file.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingCommitFailureKeepsDestination();
}

TEST(RxStaging, Class2CommitFailureKeepsDestination) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A failed Class 2 rename onto the destination keeps it; the FIN reports the data complete and the file "
        "discarded by filestore rejection.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2CommitFailureKeepsDestination();
}

TEST(RxStaging, PathFormat) {
    REQUIREMENT("CFDP-016");
    COMMENT("Staging paths are unique, refused rather than truncated, and reserved destination names are refused.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingPathFormat();
}

TEST(RxStaging, Class1MissingTrailingZerosNotCommitted) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-004");
    COMMENT("A Class 1 receive missing trailing zero bytes fails although its checksum matches.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1MissingTrailingZerosNotCommitted();
}

TEST(RxStaging, Class1MissingMiddleSegmentNotCommitted) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-004");
    COMMENT("A Class 1 receive missing a middle zero segment fails although its checksum matches.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1MissingMiddleSegmentNotCommitted();
}

TEST(RxStaging, Class1OutOfOrderSegmentsCommit) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-001");
    COMMENT("A Class 1 receive whose segments arrive out of order is committed.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1OutOfOrderSegmentsCommit();
}

TEST(RxStaging, Class1ZeroLengthCommits) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-001");
    COMMENT("A zero-length Class 1 receive is committed as an empty file.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1ZeroLengthCommits();
}

TEST(RxStaging, DestinationNamingActiveStagingFileRefused) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A destination naming another receive's active staging file is refused; that receive still commits its own "
        "data.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingDestinationNamingActiveStagingFileRefused();
}

TEST(RxStaging, DestinationNamingActiveTempFileRefused) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A destination naming another receive's active temp file is refused; that receive still commits its own data.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingDestinationNamingActiveTempFileRefused();
}

TEST(RxStaging, DestinationNamingActiveStagingFileRefusedLateMetadata) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "A Class 2 receive whose late Metadata names another receive's active staging file is refused and removes "
        "only its own temp file; the other receive still commits its own data.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingDestinationNamingActiveStagingFileRefusedLateMetadata();
}

TEST(RxStaging, ReservedNameParsing) {
    REQUIREMENT("CFDP-016");
    COMMENT(
        "Reserved staging and temp names are recognised in any directory and ignoring ASCII case; short ordinary and "
        "malformed names are allowed.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingReservedNameParsing();
}

TEST(RxStaging, Class1AbandonKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    COMMENT("An abandoned Class 1 receive removes its staging file and leaves the destination unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1AbandonKeepsExistingFile();
}

TEST(RxStaging, TeardownRemovesInProgressFile) {
    REQUIREMENT("CFDP-016");
    COMMENT("Component teardown removes an unfinished receive's staging file without emitting events.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingTeardownRemovesInProgressFile();
}

TEST(RxStaging, Class1PartialOverlapFailsChecksum) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-004");
    COMMENT("Partly overlapping Class 1 File Data is digested twice, fails the checksum, and is not committed.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1PartialOverlapFailsChecksum();
}

TEST(RxStaging, Class2InactivityKeepsExistingFile) {
    REQUIREMENT("CFDP-016");
    REQUIREMENT("CFDP-009");
    COMMENT("A Class 2 receive ended by inactivity is reported as failed and leaves the destination unchanged.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass2InactivityKeepsExistingFile();
}

TEST(RxStaging, Class1DroppedTransactionNotReported) {
    REQUIREMENT("CFDP-009");
    COMMENT("Class 1 File Data without Metadata is dropped, creates no file, and is not reported as a transfer.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testRxStagingClass1DroppedTransactionNotReported();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
