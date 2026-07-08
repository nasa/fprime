// ======================================================================
// \title  CfdpManagerEventTests.cpp
// \author Auto-generated
// \brief  cpp file for CfdpManager component event coverage tests
//
// ======================================================================

#include "CfdpManagerTester.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

namespace {

// Test helper: PDU that reports incorrect buffer size to trigger serialization failure
class OversizedTestPdu : public Cfdp::PduBase {
  private:
    U32 m_reportedSize;  // What getBufferSize() returns
    U32 m_actualSize;    // What serializeTo() actually tries to write

  public:
    OversizedTestPdu(U32 reportedSize, U32 actualSize) : m_reportedSize(reportedSize), m_actualSize(actualSize) {
        // Initialize with dummy header - we just need a valid PDU type
        this->m_header.initialize(Cfdp::PduTypeEnum::ACKNOWLEDGMENT, Cfdp::PduDirection::DIRECTION_TOWARD_SENDER,
                                  Cfdp::Class::CLASS_2,
                                  1,  // sourceEid
                                  1,  // transactionSeq
                                  1   // destEid
        );
    }

    U32 getBufferSize() const override {
        return m_reportedSize;  // Lie about size
    }

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override {
        // First serialize the header (this should succeed)
        Fw::SerializeStatus status = this->m_header.toSerialBuffer(buffer);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }

        // Now try to write more data than the buffer has space for
        // This will fail with FW_SERIALIZE_NO_ROOM_LEFT
        for (U32 i = this->m_header.getBufferSize(); i < m_actualSize; i++) {
            U8 dummyByte = static_cast<U8>(i);
            status = buffer.serializeFrom(dummyByte);
            if (status != Fw::FW_SERIALIZE_OK) {
                return status;  // Expected failure
            }
        }

        return Fw::FW_SERIALIZE_OK;
    }

    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override {
        // Not used in this test
        return Fw::FW_SERIALIZE_FORMAT_ERROR;
    }
};

// Test engine that forces the metadata-send failure branch. sSubstateSendMetadata()
// emits TxSendMetadataFailed only when sendMd() returns ERROR, which is unreachable for
// a well-formed MetadataPdu. Overriding sendMd() to return ERROR is
// the minimal seam to cover that guard; setTxnStatus()/finishTransaction() (called on
// the failure tail) fall through to the real Engine implementations.
class SendMdFailEngine : public Engine {
  public:
    explicit SendMdFailEngine(CfdpManager* mgr) : Engine(mgr) {}
    Status::T sendMd(Transaction*) override { return Cfdp::Status::ERROR; }
};

}  // anonymous namespace

// ----------------------------------------------------------------------
// Event Coverage Tests
// ----------------------------------------------------------------------
// These tests focus on triggering and verifying events not covered by
// command tests. Many events require specific failure conditions.

// ----------------------------------------------------------------------
// Transaction Activity Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testTxFileQueuedEvent() {
    // TxFileQueued event emitted when file is queued for transmission

    const char* srcFile = "test/ut/output/tx_queued_test.bin";
    const char* destFile = "/dest/tx_queued.bin";

    // Create source file
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    U8 testData[10] = {1, 2, 3};
    FwSizeType sizeToWrite = 3;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearEvents();

    // Queue file for transfer
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, 0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Verify exact event count and TxFileQueued event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TxFileQueued_SIZE(1);

    // Clean up test file
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testTxFileTransferStartedEvent() {
    // TxFileTransferStarted emitted when file transfer begins

    const char* srcFile = "test/ut/output/tx_started_test.bin";
    const char* destFile = "/dest/tx_started.bin";

    // Create source file
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE);
    U8 testData[10] = {1, 2, 3};
    FwSizeType sizeToWrite = 3;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearEvents();

    // Initiate transfer
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, 0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Run cycles to start transfer
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify TxFileTransferStarted event exists
    // Note: Multiple events emitted (TxFileQueued, TxFileTransferStarted, and potentially others)
    ASSERT_EVENTS_TxFileTransferStarted_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testMetadataReceivedEvent() {
    // MetadataReceived emitted when metadata PDU arrives

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 500;
    const char* srcFile = "/ground/metadata_test.bin";
    const char* dstFile = "test/ut/output/metadata_test_rx.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearEvents();

    // Send Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Verify exact event count and MetadataReceived event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_MetadataReceived_SIZE(1);
    // Note: Payload verification not available - no ASSERT macro with parameters generated
}

// ----------------------------------------------------------------------
// Buffer Management Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testBuffersExhaustedEvent() {
    // BuffersExhausted emitted when buffer allocation fails
    // Test by mocking buffer allocation failure and calling getPduBuffer

    // Setup a minimal transaction to get a valid channel reference
    (void)setupTestTransaction(TxnState::TXN_STATE_S1,  // Sender, class 1
                               TEST_CHANNEL_ID_0,       // Channel 0
                               "/test/source.txt",      // Source file
                               "/test/dest.txt",        // Dest file
                               1024,                    // File size
                               1,                       // Sequence ID
                               TEST_GROUND_EID);        // Peer entity ID

    // Get the channel directly from the engine
    Channel* chan = component.m_engine->m_channels[TEST_CHANNEL_ID_0];
    FW_ASSERT(chan != nullptr);

    // Clear event history before test
    this->clearHistory();

    // Set flag to simulate buffer allocation failure
    this->setFailBufferAllocation(true);

    // Attempt to allocate a buffer - should fail and emit BuffersExhausted event
    Fw::Buffer buffer;
    Status::T status = component.getPduBuffer(buffer, *chan, 100);

    // Verify allocation failed
    ASSERT_EQ(Status::SEND_PDU_NO_BUF_AVAIL_ERROR, status);

    // Verify BuffersExhausted event was emitted
    ASSERT_EVENTS_BuffersExhausted_SIZE(1);

    // Reset flag for subsequent tests
    this->setFailBufferAllocation(false);
}

// ----------------------------------------------------------------------
// PDU Deserialization Failure Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testFailPduHeaderDeserializationEvent() {
    // FailPduHeaderDeserialization emitted for malformed PDU header
    // Create a buffer with packet descriptor but truncated PDU header
    // Minimum PDU header is 7 bytes (MIN_HEADERSIZE), so use fewer bytes

    const FwSizeType descriptorSize = sizeof(FwPacketDescriptorType);
    U8 truncatedData[10];  // descriptor (2) + partial header (4)
    Fw::Buffer truncatedBuffer(truncatedData, descriptorSize + 4);

    // Write packet descriptor (2 bytes)
    FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_FILE);
    truncatedData[0] = static_cast<U8>((descriptor >> 8) & 0xFF);
    truncatedData[1] = static_cast<U8>(descriptor & 0xFF);

    // Write partial PDU header (only 4 bytes - too short for MIN_HEADERSIZE of 7)
    truncatedData[2] = 0x20;  // Flags byte
    truncatedData[3] = 0x00;  // PDU length MSB
    truncatedData[4] = 0x00;  // PDU length LSB
    truncatedData[5] = 0x00;  // eidTsnLengths byte

    this->clearEvents();

    // Send truncated PDU on channel 0 - should fail header deserialization
    this->invoke_to_dataIn(0, truncatedBuffer);

    // Dispatch the message
    this->component.doDispatch();

    // Verify FailPduHeaderDeserialization event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailPduHeaderDeserialization_SIZE(1);
    ASSERT_EVENTS_FailPduHeaderDeserialization(0,                                                 // index
                                               0,                                                 // channelId
                                               static_cast<I32>(Fw::FW_DESERIALIZE_BUFFER_EMPTY)  // status
    );
}

void CfdpManagerTester::testFailMetadataPduDeserializationEvent() {
    // FailMetadataPduDeserialization for malformed metadata PDU
    // Strategy: Send a valid metadata PDU but with truncated body

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1500;
    const char* srcFile = "/ground/metadata_deser_test.bin";
    const char* dstFile = "test/ut/output/metadata_deser_test.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // Create a Metadata PDU using the helper
    Cfdp::MetadataPdu metadataPdu;
    metadataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_1, sourceEid,
                           transactionSeq, destEid, fileSize, srcFile, dstFile,
                           Cfdp::ChecksumType::CHECKSUM_TYPE_MODULAR, 0);

    // Serialize to a buffer
    const FwSizeType descriptorSize = sizeof(FwPacketDescriptorType);
    U32 pduSize = metadataPdu.getBufferSize();
    U32 totalSize = static_cast<U32>(descriptorSize) + pduSize;

    U8 tempBuffer[300];
    Fw::Buffer pduBuffer(tempBuffer, totalSize);

    // Write packet descriptor
    U8* bufferData = pduBuffer.getData();
    FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_FILE);
    bufferData[0] = static_cast<U8>((descriptor >> 8) & 0xFF);
    bufferData[1] = static_cast<U8>(descriptor & 0xFF);

    // Serialize PDU
    Fw::SerialBuffer sb(bufferData + descriptorSize, pduSize);
    metadataPdu.serializeTo(sb);

    // Now TRUNCATE the buffer to cause deserialization failure
    // Remove last 30 bytes so it can't read the filenames
    FwSizeType truncatedSize = (descriptorSize + sb.getSize()) - 30;
    pduBuffer.setSize(truncatedSize);

    // Send the truncated PDU
    this->invoke_to_dataIn(channelId, pduBuffer);
    this->component.doDispatch();

    // Verify FailMetadataPduDeserialization event
    ASSERT_EVENTS_FailMetadataPduDeserialization_SIZE(1);
}

void CfdpManagerTester::testFailFileDataPduDeserializationEvent() {
    // FailFileDataPduDeserialization emitted when FileData PDU body is malformed
    // Strategy: Send FileData PDU with valid header but truncated body

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2100;
    const char* srcFile = "/ground/filedata_deser.bin";
    const char* dstFile = "test/ut/output/filedata_deser.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // First establish transaction with Metadata
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    this->clearEvents();

    // Create a valid FileData PDU
    Cfdp::FileDataPdu fileDataPdu;
    U8 testData[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    fileDataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_1, sourceEid,
                           transactionSeq, destEid, 0, 10, testData);  // offset, dataSize, data

    // Serialize to buffer with descriptor
    const FwSizeType descriptorSize = sizeof(FwPacketDescriptorType);
    U32 pduSize = fileDataPdu.getBufferSize();
    U32 totalSize = static_cast<U32>(descriptorSize) + pduSize;

    U8 tempBuffer[300];
    Fw::Buffer pduBuffer(tempBuffer, totalSize);

    // Write packet descriptor
    U8* bufferData = pduBuffer.getData();
    FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_FILE);
    bufferData[0] = static_cast<U8>((descriptor >> 8) & 0xFF);
    bufferData[1] = static_cast<U8>(descriptor & 0xFF);

    // Serialize PDU
    Fw::SerialBuffer sb(bufferData + descriptorSize, pduSize);
    fileDataPdu.serializeTo(sb);

    // Truncate just the file data payload (not the entire PDU header)
    // This ensures we fail at FileData body parsing, not header parsing
    FwSizeType truncatedSize = (descriptorSize + sb.getSize()) - 5;
    pduBuffer.setSize(truncatedSize);

    // Send truncated PDU
    this->invoke_to_dataIn(channelId, pduBuffer);
    this->component.doDispatch();

    // Verify FailFileDataPduDeserialization event was emitted
    // Note: Other events may be emitted during error recovery (e.g., transaction state changes)
    ASSERT_EVENTS_FailFileDataPduDeserialization_SIZE(1);
}

void CfdpManagerTester::testFailEofPduDeserializationEvent() {
    // FailEofPduDeserialization emitted when EOF PDU is malformed
    // Strategy: Establish proper RX transaction context, then send truncated EOF

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2200;
    const char* srcFile = "/ground/eof_deser.bin";
    const char* dstFile = "test/ut/output/eof_deser.bin";
    Cfdp::FileSize fileSize = 50;

    this->clearHistory();
    this->clearEvents();

    // Step 1: Establish RX transaction with Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Step 2: Send some FileData to advance transaction state
    U8 testData[50];
    for (U32 i = 0; i < 50; i++) {
        testData[i] = static_cast<U8>(i);
    }
    this->sendFileDataPdu(channelId, sourceEid, destEid, transactionSeq, 0, 50, testData, Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Now transaction is expecting EOF
    this->clearEvents();

    // Step 3: Create a valid EOF PDU
    Cfdp::EofPdu eofPdu;
    eofPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_1, sourceEid, transactionSeq,
                      destEid, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                      0x12345678,  // checksum
                      fileSize);

    // Step 4: Serialize to buffer with descriptor
    const FwSizeType descriptorSize = sizeof(FwPacketDescriptorType);
    U32 pduSize = eofPdu.getBufferSize();
    U32 totalSize = static_cast<U32>(descriptorSize) + pduSize;

    U8 tempBuffer[300];
    Fw::Buffer pduBuffer(tempBuffer, totalSize);

    // Write packet descriptor
    U8* bufferData = pduBuffer.getData();
    FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_FILE);
    bufferData[0] = static_cast<U8>((descriptor >> 8) & 0xFF);
    bufferData[1] = static_cast<U8>(descriptor & 0xFF);

    // Serialize PDU
    Fw::SerialBuffer sb(bufferData + descriptorSize, pduSize);
    eofPdu.serializeTo(sb);

    // Step 5: Truncate EOF body (not header)
    // Remove just enough to cause EOF body parsing failure
    FwSizeType truncatedSize = (descriptorSize + sb.getSize()) - 4;
    pduBuffer.setSize(truncatedSize);

    // Step 6: Send truncated EOF PDU
    this->invoke_to_dataIn(channelId, pduBuffer);
    this->component.doDispatch();

    // Run cycles to process
    for (U32 i = 0; i < 3; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify FailEofPduDeserialization event was emitted
    // Note: Other events may be emitted during error recovery
    ASSERT_EVENTS_FailEofPduDeserialization_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testFailAckPduDeserializationEvent() {
    // FailAckPduDeserialization emitted when ACK PDU deserialization fails

    // Set up minimal Class 2 TX transaction
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "dummy_src.txt",         // srcFilename
                                            "dummy_dst.txt",         // dstFilename
                                            100,                     // fileSize
                                            1,                       // sequenceId
                                            TEST_GROUND_EID          // peerId
    );

    // Create malformed buffer (too short for valid ACK PDU)
    U8 malformedData[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    Fw::Buffer malformedBuffer(malformedData, sizeof(malformedData));

    this->clearEvents();

    // Directly call s2EofAck with malformed buffer
    txn->s2EofAck(malformedBuffer);

    // Verify event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailAckPduDeserialization_SIZE(1);
}

void CfdpManagerTester::testFailFinPduDeserializationEvent() {
    // FailFinPduDeserialization emitted when FIN PDU deserialization fails

    // Set up minimal Class 2 TX transaction
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "dummy_src.txt",         // srcFilename
                                            "dummy_dst.txt",         // dstFilename
                                            100,                     // fileSize
                                            1,                       // sequenceId
                                            TEST_GROUND_EID          // peerId
    );

    // Create malformed buffer (too short for valid FIN PDU)
    U8 malformedData[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    Fw::Buffer malformedBuffer(malformedData, sizeof(malformedData));

    this->clearEvents();

    // Directly call s2Fin with malformed buffer
    txn->s2Fin(malformedBuffer);

    // Verify event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailFinPduDeserialization_SIZE(1);
}

void CfdpManagerTester::testFailNakPduDeserializationEvent() {
    // FailNakPduDeserialization emitted when NAK PDU deserialization fails

    // Set up minimal Class 2 TX transaction
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "dummy_src.txt",         // srcFilename
                                            "dummy_dst.txt",         // dstFilename
                                            100,                     // fileSize
                                            1,                       // sequenceId
                                            TEST_GROUND_EID          // peerId
    );

    // Create malformed buffer (too short for valid NAK PDU)
    U8 malformedData[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    Fw::Buffer malformedBuffer(malformedData, sizeof(malformedData));

    this->clearEvents();

    // Directly call s2Nak with malformed buffer
    txn->s2Nak(malformedBuffer);

    // Verify event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailNakPduDeserialization_SIZE(1);
}

// ----------------------------------------------------------------------
// RX Error Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testRxFileCreateFailedEvent() {
    // RxFileCreateFailed emitted when destination file creation fails

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 700;

    // Use invalid destination path (directory doesn't exist)
    const char* srcFile = "/ground/test.bin";
    const char* dstFile = "/nonexistent_dir/subdir/file.bin";
    Cfdp::FileSize fileSize = 10;

    this->clearEvents();

    // Send Metadata PDU with invalid destination
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Send file data (triggers file creation attempt)
    U8 testData[10] = {1, 2, 3};
    this->sendFileDataPdu(channelId, sourceEid, destEid, transactionSeq, 0, 3, testData, Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Run cycles to process
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify RxFileCreateFailed event
    // Note: Multiple events emitted during RX transaction setup and failure
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
}

void CfdpManagerTester::testRxCrcMismatchEvent() {
    // RxCrcMismatch emitted when received file CRC doesn't match EOF CRC

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 800;
    const char* srcFile = "/ground/crc_test.bin";
    const char* dstFile = "test/ut/output/crc_mismatch_rx.bin";

    U8 testData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Cfdp::FileSize fileSize = sizeof(testData);

    this->clearEvents();

    // Send Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_2, 1);  // Class 2 for CRC checking
    this->component.doDispatch();

    // Send File Data PDU
    this->sendFileDataPdu(channelId, sourceEid, destEid, transactionSeq, 0, static_cast<U16>(fileSize), testData,
                          Cfdp::Class::CLASS_2);
    this->component.doDispatch();

    // Send EOF with WRONG checksum (should cause mismatch)
    U32 wrongChecksum = 0xDEADBEEF;  // Intentionally wrong
    this->sendEofPdu(channelId, sourceEid, destEid, transactionSeq, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                     wrongChecksum, fileSize, Cfdp::Class::CLASS_2);
    this->component.doDispatch();

    // Run cycles to complete CRC calculation and verification
    for (U32 i = 0; i < 20; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify RxCrcMismatch event
    // Note: Multiple events emitted during RX transaction (Metadata, potential completion events)
    ASSERT_EVENTS_RxCrcMismatch_SIZE(1);

    // Note: RxFileTransferFailed may not be emitted immediately after CRC mismatch
    // It requires the transaction to fully complete/finalize which may need additional cycles
    // or specific PDU sequencing. Consider testing in a more complete transaction scenario.
}

void CfdpManagerTester::testRxFileSizeMismatchEvent() {
    // RxFileSizeMismatch emitted when metadata size != EOF size

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 900;
    const char* srcFile = "/ground/size_test.bin";
    const char* dstFile = "test/ut/output/size_mismatch_rx.bin";

    Cfdp::FileSize metadataSize = 1000;
    Cfdp::FileSize eofSize = 500;  // Different!

    this->clearEvents();

    // Send Metadata PDU claiming file is 1000 bytes
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, metadataSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Send EOF claiming file is 500 bytes (mismatch!)
    U32 checksum = 0;
    this->sendEofPdu(channelId, sourceEid, destEid, transactionSeq, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                     checksum, eofSize, Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Run cycles
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify RxFileSizeMismatch event
    // Note: Multiple events emitted (MetadataReceived, size mismatch detection, etc.)
    ASSERT_EVENTS_RxFileSizeMismatch_SIZE(1);
}

void CfdpManagerTester::testRxEofCancelReceivedEvent() {
    // RxEofCancelReceived emitted when EOF has CANCEL condition code

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1000;
    const char* srcFile = "/ground/cancel_test.bin";
    const char* dstFile = "test/ut/output/cancel_rx.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearEvents();

    // Send Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Send EOF with CANCEL_REQUEST_RECEIVED condition code
    U32 checksum = 0;
    this->sendEofPdu(channelId, sourceEid, destEid, transactionSeq,
                     Cfdp::ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED, checksum, fileSize,
                     Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Run cycles
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify RxEofCancelReceived event
    // Note: Multiple events emitted (MetadataReceived, RxEofCancelReceived, potentially RxFileTransferFailed)
    ASSERT_EVENTS_RxEofCancelReceived_SIZE(1);
}

void CfdpManagerTester::testRxEofWithErrorEvent() {
    // RxEofWithError emitted when EOF has non-zero condition code (not CANCEL)

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1700;
    const char* srcFile = "/ground/eof_error_test.bin";
    const char* dstFile = "test/ut/output/eof_error_test.bin";
    Cfdp::FileSize fileSize = 50;

    this->clearHistory();
    this->clearEvents();

    // Send Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    this->clearEvents();

    // Send EOF with CHECK_LIMIT_REACHED error condition
    U32 checksum = 0;
    this->sendEofPdu(channelId, sourceEid, destEid, transactionSeq,
                     Cfdp::ConditionCode::CONDITION_CODE_CHECK_LIMIT_REACHED, checksum, fileSize, Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Run cycles
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify RxEofWithError event
    ASSERT_EVENTS_RxEofWithError_SIZE(1);
}

void CfdpManagerTester::testRxEofMdSizeMismatchEvent() {
    // RxEofMdSizeMismatch emitted when metadata file size != EOF file size
    // Strategy: Set up R2 transaction with EOF already received, then send
    // metadata with different file size

    // Set up Class 2 receiver transaction
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_R2,  // Class 2 receiver
                                            0,                       // channelId
                                            "test_src.dat",          // srcFilename
                                            "test_dst.dat",          // dstFilename
                                            1000,                    // fileSize (metadata will claim this)
                                            1,                       // sequenceId
                                            TEST_GROUND_EID          // peerId
    );

    // Simulate EOF already received with DIFFERENT size
    txn->m_flags.rx.eof_recv = true;
    txn->m_flags.rx.md_recv = false;               // Metadata not yet received
    txn->m_state_data.receive.r2.eof_size = 2000;  // Mismatch: 2000 != 1000

    // Ensure engine is set (should already be set from setupTestTransaction)
    txn->m_engine = this->component.m_engine;

    // Create Metadata PDU with file size = 1000
    Cfdp::MetadataPdu metadataPdu;
    metadataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                           TEST_GROUND_EID,                     // sourceEid
                           1,                                   // transactionSeq
                           this->component.getLocalEidParam(),  // destEid
                           1000,                                // fileSize (different from eof_size)
                           "test_src.dat", "test_dst.dat", Cfdp::ChecksumType::CHECKSUM_TYPE_MODULAR,
                           1  // closureRequested for Class 2
    );

    // Serialize PDU to buffer
    U8 tempBuffer[300];
    Fw::SerialBuffer sb(tempBuffer, sizeof(tempBuffer));
    metadataPdu.serializeTo(sb);

    Fw::Buffer pduBuffer(tempBuffer, sb.getSize());

    this->clearEvents();

    // Directly call r2RecvMd
    txn->r2RecvMd(pduBuffer);

    // Verify events emitted:
    // 1. MetadataReceived (from Engine::recvMd)
    // 2. RxEofMdSizeMismatch (from size mismatch check)
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_MetadataReceived_SIZE(1);
    ASSERT_EVENTS_RxEofMdSizeMismatch_SIZE(1);
}

void CfdpManagerTester::testRxTransactionLimitReachedEvent() {
    // RxTransactionLimitReached emitted when all transaction slots exhausted
    // Strategy: Fill up all CFDP_NUM_TRANSACTIONS_PER_CHANNEL slots, then send one more PDU
    //
    // Note: CFDP_NUM_TRANSACTIONS_PER_CHANNEL is the total pool size shared by TX and RX.
    // From CfdpCfg.hpp:
    //   = CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN (10)
    //   + CFDP_MAX_SIMULTANEOUS_RX (5)
    //   + (CFDP_MAX_POLLING_DIR_PER_CHAN + CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) *
    //   CFDP_NUM_TRANSACTIONS_PER_PLAYBACK = 10 + 5 + (5 + 2) * 5 = 50

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    const char* srcFile = "/ground/rx_limit.bin";
    const char* dstFile = "test/ut/output/rx_limit_test.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // Exhaust all transaction slots in the FREE pool
    // Channel::findUnusedTransaction() pulls from the shared FREE pool regardless of direction
    for (U32 i = 0; i < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; i++) {
        this->sendMetadataPdu(channelId, sourceEid, destEid,
                              1000 + i,  // Unique transaction sequence
                              fileSize, srcFile, dstFile, Cfdp::Class::CLASS_1, 1);
        this->component.doDispatch();
    }

    this->clearEvents();

    // Send one more PDU - this should trigger RxTransactionLimitReached
    // because all transaction slots (FREE pool) are exhausted
    this->sendMetadataPdu(channelId, sourceEid, destEid,
                          9999,  // Different transaction sequence
                          fileSize, srcFile, dstFile, Cfdp::Class::CLASS_1, 1);
    this->component.doDispatch();

    // Verify the event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_RxTransactionLimitReached_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testRxInvalidDirectiveCodeEvent() {
    // RxInvalidDirectiveCode CANNOT be unit tested with current implementation
    //
    // ISSUE: The event is emitted when directiveCode >= FILE_DIRECTIVE_INVALID_MAX (13),
    // but peekPduType() filters PDUs with unknown directive codes (returning NONE)
    // BEFORE they reach the validation logic in rDispatchRecv().
    //
    // CODE FLOW:
    // 1. dataIn_handler() receives PDU buffer
    // 2. Engine::receivePdu() calls peekPduType() to identify PDU type
    // 3. peekPduType() (PduHeader.cpp:230-284) reads directive code and maps via switch:
    //    - Recognizes: 4 (EOF), 5 (FIN), 6 (ACK), 7 (METADATA), 8 (NAK)
    //    - Returns NONE for all other codes (including >= 13)
    // 4. TransactionRx.cpp:1120 checks: if (pduType != NONE) { validate directive }
    // 5. Invalid codes filtered out, validation never runs, event never emitted
    //
    // ROOT CAUSE: peekPduType() serves dual purposes:
    // - Type identification for dispatch routing
    // - Implicit filtering of unknown codes
    // This prevents proper error reporting for protocol violations.
    //
    // SOLUTIONS:
    // - Short-term: Integration test with protocol-level PDU injection
    // - Long-term: Refactor peekPduType() to pass through unknown directive codes
    //              and let rDispatchRecv() validation emit appropriate events
    //
    // TEST STATUS: Marked as not implemented due to architectural limitation
}

void CfdpManagerTester::testRxInvalidEofPduEvent() {
    // RxInvalidEofPdu is DEAD CODE - never emitted in current implementation
    //
    // This event is only emitted when Engine::recvEof() returns ERROR status,
    // but recvEof() ALWAYS returns SUCCESS (see Engine.cpp implementation).
    //
    // Event exists in Events.fppi but is unreachable with current validation logic.
    //
    // If EOF validation logic is added to Engine::recvEof() in the future,
    // this test should send an EOF PDU with internally inconsistent fields
    // (e.g., file size doesn't match received data) and assert:
    // ASSERT_EVENTS_RxInvalidEofPdu_SIZE(1);
    //
    // Note: RxEofWithError is a different event (triggered by non-zero condition code)
    // and is already tested by testRxEofWithErrorEvent().
}

void CfdpManagerTester::testRxInactivityTimeoutEvent() {
    // RxInactivityTimeout emitted when RX transaction sits idle too long
    // Strategy: Establish RX transaction, then directly call rSendInactivityEvent()

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2400;
    const char* srcFile = "/ground/rx_timeout.bin";
    const char* dstFile = "test/ut/output/rx_timeout.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // Establish RX transaction with metadata
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_2, 1);
    this->component.doDispatch();

    this->clearEvents();

    // Find the transaction
    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Directly trigger inactivity timeout event
    txn->rSendInactivityEvent();

    // Verify event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_RxInactivityTimeout_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testRxAckLimitReachedEvent() {
    // RxAckLimitReached emitted when ACK retry limit exceeded
    // Strategy: Create Class 2 RX transaction, set state to CLOSEOUT_SYNC with
    // acknak_count at limit-1, expire timer, then call rAckTimerTick() to trigger event

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2500;
    const char* srcFile = "/ground/ack_limit.bin";
    const char* dstFile = "test/ut/output/ack_limit.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // Establish Class 2 RX transaction with metadata
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_2, 1);
    this->component.doDispatch();

    this->clearEvents();

    // Find the transaction
    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Set up state for ACK limit scenario:
    // - Transaction must be in CLOSEOUT_SYNC (waiting for FIN-ACK)
    // - Transaction must be complete
    // - ACK timer must be armed
    // - acknak_count must be at limit-1 (so next tick triggers the event)
    txn->m_state_data.receive.sub_state = RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC;
    txn->m_flags.rx.complete = true;
    txn->m_flags.com.ack_timer_armed = true;

    // Set acknak_count to one less than the limit
    U8 ack_limit = this->component.getAckLimitParam(channelId);
    txn->m_state_data.receive.r2.acknak_count = ack_limit - 1;

    // Arm the timer and make it expire
    txn->m_ack_timer.setTimer(1);
    txn->m_ack_timer.run();  // Expires the timer (secondsRemaining goes to 0)
    ASSERT_EQ(txn->m_ack_timer.getStatus(), Timer::EXPIRED) << "Timer should be expired";

    // Call the timer tick function - this will increment acknak_count to the limit
    // and emit the RxAckLimitReached event, followed by RxFileTransferFailed
    // when finishTransaction is called
    txn->rAckTimerTick();

    // Verify events: should get RxAckLimitReached + RxFileTransferFailed
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_RxAckLimitReached_SIZE(1);
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testRxNakLimitReachedEvent() {
    // RxNakLimitReached emitted when NAK retry limit exceeded
    // Strategy: Create Class 2 RX transaction with file data gaps, set acknak_count
    // to limit-1, then call r2Complete() to trigger the event

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2600;
    const char* srcFile = "/ground/nak_limit.bin";
    const char* dstFile = "test/ut/output/nak_limit.bin";
    Cfdp::FileSize fileSize = 1000;

    this->clearHistory();
    this->clearEvents();

    // Establish Class 2 RX transaction with metadata
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_2, 1);
    this->component.doDispatch();

    this->clearEvents();

    // Find the transaction
    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Set up state for NAK limit scenario:
    // - Metadata has been received
    // - File has gaps (partial data received, not EOF yet)
    // - acknak_count must be at limit-1 (so next r2Complete triggers the event)
    txn->m_flags.rx.md_recv = true;
    txn->m_flags.rx.eof_recv = false;  // No EOF yet, so gaps remain

    // Send partial file data to create gaps
    // Send data for offset 0-99, leaving gaps at 100-999
    U8 testData[100];
    memset(testData, 0xAB, sizeof(testData));
    this->sendFileDataPdu(channelId, sourceEid, destEid, transactionSeq, 0, 100, testData, Cfdp::Class::CLASS_2);
    this->component.doDispatch();

    this->clearEvents();

    // Set acknak_count to one less than the limit
    U8 nak_limit = this->component.getNackLimitParam(channelId);
    txn->m_state_data.receive.r2.acknak_count = nak_limit - 1;

    // Call r2Complete - this will detect gaps, increment acknak_count to the limit,
    // and emit the RxNakLimitReached event
    txn->r2Complete(true);

    // Verify event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_RxNakLimitReached_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testUnhandledPduInIdleStateEvent() {
    // UnhandledPduInIdleState emitted when PDU received with no matching transaction
    // Send an EOF PDU without first establishing a transaction (no Metadata sent)

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1200;
    Cfdp::ConditionCode conditionCode = Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR;
    U32 checksum = 0x12345678;
    Cfdp::FileSize fileSize = 100;

    this->clearEvents();

    // Send EOF PDU with no prior Metadata PDU (no active transaction)
    // This is an unexpected PDU type in INIT state
    this->sendEofPdu(channelId, sourceEid, destEid, transactionSeq, conditionCode, checksum, fileSize,
                     Cfdp::Class::CLASS_1);
    this->component.doDispatch();

    // Verify UnhandledPduInIdleState event
    ASSERT_EVENTS_UnhandledPduInIdleState_SIZE(1);
}

void CfdpManagerTester::testInvalidDestinationEidEvent() {
    // InvalidDestinationEid emitted when PDU destination doesn't match LocalEid

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId wrongDestEid = 999;  // Wrong destination!
    Cfdp::TransactionSeq transactionSeq = 1100;
    const char* srcFile = "/ground/wrong_dest.bin";
    const char* dstFile = "test/ut/output/wrong_dest.bin";
    Cfdp::FileSize fileSize = 50;

    this->clearEvents();

    // Send Metadata PDU with wrong destination EID
    this->sendMetadataPdu(channelId, sourceEid, wrongDestEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    // Verify exact event count and InvalidDestinationEid event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidDestinationEid_SIZE(1);
    // Note: Payload verification not available for this event - no ASSERT macro generated
}

// ----------------------------------------------------------------------
// TX Error Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testTxZeroLengthFileEvent() {
    // TxZeroLengthFile already covered by Command.SendFileZeroLength
    // This test documents that the event is tested
}

void CfdpManagerTester::testTxFileOpenFailedEvent() {
    // TxFileOpenFailed already covered by Command.SendFileNonExistent
    // This test documents that the event is tested in command tests
}

void CfdpManagerTester::testTxInvalidDirectiveCodeEvent() {
    // TxInvalidDirectiveCode emitted when invalid directive code received during TX
    //
    // ANALYSIS: This event is difficult to trigger in unit tests due to the code structure.
    //
    // The event emission logic is in TransactionTx.cpp sDispatchRecv():
    //   pduType = peekPduType(buffer);
    //   if (pduType == FILE_DATA) {
    //       // emit different event
    //   } else if (pduType != NONE) {
    //       // parse header and directive code
    //       if (directiveCode >= FILE_DIRECTIVE_INVALID_MAX) {
    //           log_WARNING_LO_TxInvalidDirectiveCode(...);  // <-- Event emitted here
    //       }
    //   }
    //
    // The problem: peekPduType() reads the directive code to determine the PDU type.
    // For unrecognized directive codes (including >= 13), it returns PduTypeEnum::NONE.
    // When pduType is NONE, the else-if block is skipped, so the event is never emitted.
    //
    // This creates an unreachable code path: to emit the event, we need directiveCode >= 13,
    // but such codes cause peekPduType to return NONE, which skips the event emission logic.
    //
    // CONCLUSION: This event can only be triggered in scenarios where PDUs bypass peekPduType,
    // or in integration tests with actual malformed PDUs from external sources.
    //
    // The event definition exists and is correct in Events.fppi. The emission logic exists
    // in TransactionTx.cpp line 742-744. It's theoretically reachable but not through normal
    // unit test PDU construction.
    //
    // Recommend: Test via integration tests with malformed PDU injection, or accept that
    // this edge case is covered by code review rather than automated testing.
}

void CfdpManagerTester::testTxEarlyFinReceivedEvent() {
    // TxEarlyFinReceived emitted when FIN arrives before EOF sent

    // Set up Class 2 TX transaction
    const U32 testSequenceId = 42;
    const U32 testSrcEid = component.getLocalEidParam();
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "test_src.txt",          // srcFilename
                                            "test_dst.txt",          // dstFilename
                                            1000,                    // fileSize
                                            testSequenceId,          // sequenceId
                                            TEST_GROUND_EID          // peerId
    );
    ASSERT_NE(txn, nullptr);

    // Set source entity ID in history (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = testSrcEid;

    // Create valid FIN PDU buffer
    Cfdp::FinPdu finPdu;
    finPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_SENDER,        // FIN is sent from receiver to sender
                      Cfdp::Class::CLASS_2,                               // FIN is only used in Class 2
                      TEST_GROUND_EID,                                    // sourceEid (from receiver)
                      testSequenceId,                                     // transactionSeq
                      testSrcEid,                                         // destEid (to sender)
                      Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,       // conditionCode
                      Cfdp::FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE,  // deliveryCode
                      Cfdp::FinFileStatus::FIN_FILE_STATUS_RETAINED       // fileStatus
    );

    // Serialize FIN PDU to buffer
    U8 finPduBuffer[256];
    Fw::SerialBuffer sb(finPduBuffer, sizeof(finPduBuffer));
    Fw::SerializeStatus status = finPdu.serializeTo(sb);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
    Fw::Buffer finBuffer(finPduBuffer, sb.getSize());

    this->clearEvents();

    // Directly call s2EarlyFin with valid FIN PDU buffer
    txn->s2EarlyFin(finBuffer);

    // Verify TxEarlyFinReceived event was emitted
    // Note: s2EarlyFin calls s2Fin which may emit additional events, so we only check for our event
    ASSERT_EVENTS_TxEarlyFinReceived_SIZE(1);
    ASSERT_EVENTS_TxEarlyFinReceived(0, Cfdp::Class::CLASS_2, testSrcEid, testSequenceId);
}

void CfdpManagerTester::testTxNonFileDirectivePduReceivedEvent() {
    // TxNonFileDirectivePduReceived emitted when file data PDU sent to TX transaction

    // 1. Set up Class 2 TX transaction
    const U32 testSequenceId = 42;
    const U32 testSrcEid = component.getLocalEidParam();
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "test_src.txt",          // srcFilename
                                            "test_dst.txt",          // dstFilename
                                            1000,                    // fileSize
                                            testSequenceId,          // sequenceId
                                            TEST_GROUND_EID          // peerId
    );
    ASSERT_NE(txn, nullptr);

    // 2. Set source entity ID in history (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = testSrcEid;

    // 3. Create File Data PDU (protocol violation - file data to sender!)
    U8 testData[] = {0x01, 0x02, 0x03, 0x04};  // Dummy file data
    Cfdp::FileDataPdu fileDataPdu;
    fileDataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_SENDER,  // Going to sender (TX)
                           Cfdp::Class::CLASS_2,                         // Class 2
                           TEST_GROUND_EID,                              // sourceEid (from receiver)
                           testSequenceId,                               // transactionSeq
                           testSrcEid,                                   // destEid (to sender)
                           0,                                            // offset
                           sizeof(testData),                             // dataSize
                           testData                                      // data
    );

    // 4. Serialize File Data PDU to buffer
    U8 pduBuffer[256];
    Fw::SerialBuffer sb(pduBuffer, sizeof(pduBuffer));
    Fw::SerializeStatus status = fileDataPdu.serializeTo(sb);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
    Fw::Buffer fdBuffer(pduBuffer, sb.getSize());

    this->clearEvents();

    // 5. Call sDispatchRecv directly (bypasses routing, triggers event check)
    txn->sDispatchRecv(fdBuffer, nullptr);

    // 6. Verify TxNonFileDirectivePduReceived event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TxNonFileDirectivePduReceived_SIZE(1);
    ASSERT_EVENTS_TxNonFileDirectivePduReceived(0,                     // index
                                                Cfdp::Class::CLASS_2,  // cfdpClass
                                                testSrcEid,            // srcEid
                                                testSequenceId         // seqNum
    );
}

void CfdpManagerTester::testTxInvalidNakPduEvent() {
    // TxInvalidNakPdu emitted when NAK with zero segments received during Class 2 TX

    // 1. Set up Class 2 TX transaction
    const U32 testSequenceId = 42;
    const U32 testSrcEid = component.getLocalEidParam();
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "test_src.txt",          // srcFilename
                                            "test_dst.txt",          // dstFilename
                                            1000,                    // fileSize
                                            testSequenceId,          // sequenceId
                                            TEST_GROUND_EID          // peerId
    );
    ASSERT_NE(txn, nullptr);

    // 2. Set source entity ID in history (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = testSrcEid;

    // 3. Ensure engine is set (required for recvNak call)
    txn->m_engine = this->component.m_engine;

    // 4. Create NAK PDU with zero segments (invalid condition)
    Cfdp::NakPdu nakPdu;
    nakPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_SENDER,  // NAK goes from receiver to sender
                      Cfdp::Class::CLASS_2,                         // NAK only used in Class 2
                      TEST_GROUND_EID,                              // sourceEid (from receiver)
                      testSequenceId,                               // transactionSeq
                      testSrcEid,                                   // destEid (to sender)
                      0,                                            // scopeStart
                      1000                                          // scopeEnd (file size)
    );
    // DO NOT call nakPdu.addSegment() - leave segments at 0 (this is the invalid condition)

    // 5. Serialize NAK PDU to buffer
    U8 nakPduBuffer[256];
    Fw::SerialBuffer sb(nakPduBuffer, sizeof(nakPduBuffer));
    Fw::SerializeStatus status = nakPdu.serializeTo(sb);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
    Fw::Buffer nakBuffer(nakPduBuffer, sb.getSize());

    this->clearEvents();

    // 6. Directly call s2Nak with NAK PDU containing zero segments
    txn->s2Nak(nakBuffer);

    // 7. Verify TxInvalidNakPdu event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TxInvalidNakPdu_SIZE(1);
    ASSERT_EVENTS_TxInvalidNakPdu(0,                     // index
                                  Cfdp::Class::CLASS_2,  // cfdpClass
                                  testSrcEid,            // srcEid
                                  testSequenceId         // seqNum
    );
}

void CfdpManagerTester::testTxInvalidSegmentRequestsEvent() {
    // TxInvalidSegmentRequests emitted when NAK has invalid segment ranges

    // 1. Set up Class 2 TX transaction
    const U32 testSequenceId = 43;
    const U32 testSrcEid = component.getLocalEidParam();
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            0,                       // channelId
                                            "test_src.txt",          // srcFilename
                                            "test_dst.txt",          // dstFilename
                                            1000,                    // fileSize (used for overflow validation)
                                            testSequenceId,          // sequenceId
                                            TEST_GROUND_EID          // peerId
    );
    ASSERT_NE(txn, nullptr);

    // 2. Set source entity ID in history (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = testSrcEid;

    // 3. Ensure engine is set (required for recvNak call)
    txn->m_engine = this->component.m_engine;

    // 4. Initialize chunks (required for NAK processing)
    Channel* chan = this->component.m_engine->m_channels[0];
    ASSERT_NE(chan, nullptr);
    txn->m_chunks = chan->findUnusedChunks(Direction::DIRECTION_TX);

    // 5. Create NAK PDU with mix of valid and invalid segments
    Cfdp::NakPdu nakPdu;
    nakPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_SENDER,  // NAK goes from receiver to sender
                      Cfdp::Class::CLASS_2,                         // NAK only used in Class 2
                      TEST_GROUND_EID,                              // sourceEid (from receiver)
                      testSequenceId,                               // transactionSeq
                      testSrcEid,                                   // destEid (to sender)
                      0,                                            // scopeStart
                      1000                                          // scopeEnd (file size)
    );

    // Add segments: mix of valid and invalid
    // Valid: (0, 0) - triggers metadata resend
    nakPdu.addSegment(0, 0);

    // Valid: normal gap
    nakPdu.addSegment(100, 200);

    // INVALID: reversed range (end < start)
    nakPdu.addSegment(500, 100);

    // INVALID: overflow (end > fileSize of 1000)
    nakPdu.addSegment(800, 2000);

    // Valid: normal gap
    nakPdu.addSegment(600, 700);

    // 6. Serialize NAK PDU to buffer
    U8 nakPduBuffer[256];
    Fw::SerialBuffer sb(nakPduBuffer, sizeof(nakPduBuffer));
    Fw::SerializeStatus status = nakPdu.serializeTo(sb);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
    Fw::Buffer nakBuffer(nakPduBuffer, sb.getSize());

    this->clearEvents();

    // 7. Directly call s2Nak with NAK PDU containing invalid segments
    txn->s2Nak(nakBuffer);

    // 8. Verify TxInvalidSegmentRequests event was emitted with badCount = 2
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TxInvalidSegmentRequests_SIZE(1);
    ASSERT_EVENTS_TxInvalidSegmentRequests(0,                     // index
                                           Cfdp::Class::CLASS_2,  // cfdpClass
                                           testSrcEid,            // srcEid
                                           testSequenceId,        // seqNum
                                           2                      // badCount (reversed + overflow)
    );
}

void CfdpManagerTester::testTxInactivityTimeoutEvent() {
    // TxInactivityTimeout emitted when TX transaction sits idle too long
    // Strategy: Create Class 2 TX transaction in TXN_STATE_S2, configure
    // inactivity timer to expire during sTick(), then verify event emission

    // Define test parameters
    U8 channelId = 0;
    const char* srcFile = "test/ut/output/tx_inactivity.bin";
    const char* dstFile = "/ground/tx_inactivity.bin";
    U32 fileSize = 500;
    U32 sequenceId = 3000;
    U32 peerId = TEST_GROUND_EID;
    U32 localEid = this->component.getLocalEidParam();

    this->clearHistory();
    this->clearEvents();

    // Create Class 2 TX transaction in S2 state
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender state
                                            channelId, srcFile, dstFile, fileSize, sequenceId, peerId);
    ASSERT_NE(txn, nullptr) << "Transaction should be created";

    // Set source entity ID (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = localEid;

    // Set engine pointer (required for setTxnStatus call in sTick)
    txn->m_engine = this->component.m_engine;

    // Ensure flags are in correct initial state for event emission
    txn->m_flags.com.inactivity_fired = false;  // Must not have fired yet
    txn->m_flags.com.canceled = false;          // Must not be canceled
    txn->m_flags.com.suspended = false;         // For good measure

    // Set send_eof flag to prevent transaction recycling after event emission
    // (sTick will try to recycle if no pending sends)
    txn->m_flags.tx.send_eof = true;

    // Set timer to 1 second - will expire when sTick calls run()
    txn->m_inactivity_timer.setTimer(1);
    ASSERT_EQ(txn->m_inactivity_timer.getStatus(), Timer::RUNNING) << "Timer should be RUNNING before sTick()";

    this->clearEvents();

    // Call sTick() - will expire timer and emit event
    I32 cont = 0;
    txn->sTick(&cont);

    // Verify TxInactivityTimeout event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TxInactivityTimeout_SIZE(1);

    // Verify event parameters are correct
    ASSERT_EVENTS_TxInactivityTimeout(0,                     // index
                                      Cfdp::Class::CLASS_2,  // cfdpClass
                                      localEid,              // srcEid
                                      sequenceId             // seqNum
    );
}

void CfdpManagerTester::testTxAckLimitReachedEvent() {
    // TxAckLimitReached emitted when EOF-ACK retry limit exceeded
    // Strategy: Create Class 2 TX transaction in CLOSEOUT_SYNC, set acknak_count
    // to limit-1, expire timer, then call sAckTimerTick() to trigger event

    U8 channelId = 0;
    const char* srcFile = "test/ut/output/tx_ack_limit.bin";
    const char* dstFile = "/ground/tx_ack_limit.bin";
    Cfdp::FileSize fileSize = 500;
    Cfdp::TransactionSeq transactionSeq = 3100;
    U32 localEid = this->component.getLocalEidParam();

    this->clearHistory();
    this->clearEvents();

    // Step 1: Create Class 2 TX transaction
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            channelId, srcFile, dstFile, fileSize, transactionSeq,
                                            TEST_GROUND_EID  // peerId
    );
    ASSERT_NE(txn, nullptr) << "Transaction should exist";

    // Step 2: Set source entity ID (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = localEid;

    // Step 3: Set engine pointer (required for setTxnStatus and finishTransaction)
    txn->m_engine = this->component.m_engine;

    // Step 4: Configure state for ACK limit scenario:
    // - Transaction must be in CLOSEOUT_SYNC (waiting for EOF-ACK)
    // - ACK timer must be armed
    // - acknak_count must be AT the limit (TX checks limit first, then increments on retry)
    txn->m_state_data.send.sub_state = TxSubState::TX_SUB_STATE_CLOSEOUT_SYNC;
    txn->m_flags.com.ack_timer_armed = true;

    // Set acknak_count to the limit (TX version checks `>=` before incrementing)
    // This differs from RX version which increments first, then checks
    U8 ack_limit = this->component.getAckLimitParam(channelId);
    txn->m_state_data.send.s2.acknak_count = ack_limit;

    // Step 5: Arm the timer with 1 second duration
    txn->m_ack_timer.setTimer(1);
    ASSERT_EQ(txn->m_ack_timer.getStatus(), Timer::RUNNING) << "Timer should be RUNNING";

    this->clearEvents();

    // Step 6: Call sAckTimerTick() FIRST time - expires the timer
    // The function sees timer is RUNNING, calls run(), which expires it
    // Then it returns (doesn't enter the else-if block on this call)
    txn->sAckTimerTick();
    ASSERT_EQ(txn->m_ack_timer.getStatus(), Timer::EXPIRED) << "Timer should be EXPIRED after first tick";

    // Step 7: Call sAckTimerTick() SECOND time - triggers the limit check
    // Now timer is not RUNNING, so it enters the else-if block
    // Checks acknak_count >= limit (true), emits events
    txn->sAckTimerTick();

    // Step 7: Verify events: should get TxAckLimitReached + TxFileTransferFailed
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_TxAckLimitReached_SIZE(1);
    ASSERT_EVENTS_TxFileTransferFailed_SIZE(1);

    // Step 8: Verify event parameters are correct
    ASSERT_EVENTS_TxAckLimitReached(0,                     // index
                                    Cfdp::Class::CLASS_2,  // cfdpClass
                                    localEid,              // srcEid
                                    transactionSeq         // seqNum
    );
}

void CfdpManagerTester::testMaxTxTransactionsReachedEvent() {
    // MaxTxTransactionsReached emitted when commanded TX file limit reached

    U8 channelId = TEST_CHANNEL_ID_0;
    const char* srcFile = "test/ut/output/tx_limit_test.bin";
    const char* destFile = "/dest/tx_limit.bin";

    this->clearHistory();
    this->clearEvents();

    // Directly set the channel's commanded TX counter to the limit
    // This simulates having CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN (10)
    // active commanded TX transactions
    Channel* chan = this->component.m_engine->m_channels[channelId];
    FW_ASSERT(chan != nullptr);
    chan->m_numCmdTx = CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN;

    // Try to send one more file - should trigger MaxTxTransactionsReached
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_1, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Verify the event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_MaxTxTransactionsReached_SIZE(1);
}

// ----------------------------------------------------------------------
// File Management Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testFileRemoveFailedEvent() {
    // FileRemoveFailed emitted when file deletion fails after transfer
    // Strategy: Establish a transaction, then call handleNotKeepFile() with
    // a file that doesn't exist or is in a protected location

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 3000;
    const char* srcFile = "/ground/remove_test.bin";
    const char* dstFile = "test/ut/output/remove_test_rx.bin";
    Cfdp::FileSize fileSize = 50;

    this->clearHistory();
    this->clearEvents();

    // Start RX transaction to get a real transaction object
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);  // 0 = DELETE (not KEEP)
    this->component.doDispatch();

    // Find the transaction
    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    this->clearEvents();

    // Modify transaction to point to a file in a protected/nonexistent location
    // Use /root which typically requires root permissions to write/delete
    txn->m_history->fnames.dst_filename = "/root/protected_file_that_cannot_be_deleted.bin";
    txn->m_history->dir = Direction::DIRECTION_RX;  // RX uses dst_filename

    // Directly call handleNotKeepFile which will try to delete the protected file
    this->component.m_engine->handleNotKeepFile(txn);

    // Verify FileRemoveFailed event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileRemoveFailed_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testPlaybackDirOpenFailedEvent() {
    // PlaybackDirOpenFailed already covered by Command.PlaybackDirectoryOpenFailed
    // Explicit test for completeness

    Fw::String nonexistentDir("/nonexistent/playback/dir");
    Fw::String destPath("/dest");

    this->clearEvents();

    // Try to playback nonexistent directory
    this->sendCmd_PlaybackDirectory(0, 0, 0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, Cfdp::Keep::DELETE, 0,
                                    nonexistentDir, destPath);
    this->component.doDispatch();

    // Run cycles to trigger directory open
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify exact event count and PlaybackDirOpenFailed event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PlaybackDirOpenFailed_SIZE(1);
}

// ----------------------------------------------------------------------
// Transaction Management Events (already covered by command tests)
// ----------------------------------------------------------------------

void CfdpManagerTester::testTransactionSuspendedEvent() {
    // TransactionSuspended already covered by Command.SuspendResumeTransactionNominal
    // Included here for event coverage completeness tracking
}

void CfdpManagerTester::testTransactionResumedEvent() {
    // TransactionResumed already covered by Command.SuspendResumeTransactionNominal
}

void CfdpManagerTester::testTransactionCanceledEvent() {
    // TransactionCanceled already covered by Command.CancelTransactionNominal
}

void CfdpManagerTester::testTransactionAbandonedEvent() {
    // TransactionAbandoned already covered by Command.AbandonTransactionNominal
}

void CfdpManagerTester::testTransactionNotFoundEvent() {
    // TransactionNotFound already covered by Command.SuspendResumeTransactionNotFound
}

// ----------------------------------------------------------------------
// Critical Error Path Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testRxWriteFailedEvent() {
    // RxWriteFailed emitted when file write fails during FileData PDU processing

    const char* srcFile = "/ground/rx_write_fail_test.bin";
    const char* dstFile = "test/ut/output/rx_write_fail_test.bin";
    TransactionSetup setup;

    // Setup RX transaction receiving file data
    setupRxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, 100, 600,
                       TxnState::TXN_STATE_R1, setup);

    // Close the file to cause write failure
    setup.txn->m_fd.close();

    this->clearEvents();

    // Send FileData PDU - write will fail
    U8 testData[32] = {1, 2, 3};
    sendFileDataPdu(TEST_CHANNEL_ID_0, TEST_GROUND_EID, component.getLocalEidParam(), 600, 0, 32, testData,
                    Cfdp::Class::CLASS_1);
    component.doDispatch();

    // Verify RxWriteFailed event
    // Note: Additional events may be emitted due to transaction state changes after write failure
    ASSERT_EVENTS_RxWriteFailed_SIZE(1);
}

void CfdpManagerTester::testRxSeekFailedEvent() {
    // RxSeekFailed emitted when file seek fails before writing data at offset

    const char* srcFile = "/ground/rx_seek_fail_test.bin";
    const char* dstFile = "test/ut/output/rx_seek_fail_test.bin";
    TransactionSetup setup;

    // Setup RX transaction with closed file to cause seek failure
    setupRxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, 100, 700,
                       TxnState::TXN_STATE_R1, setup);

    // Close file to cause seek failure
    setup.txn->m_fd.close();

    this->clearEvents();

    // Send FileData PDU with non-zero offset requiring seek
    U8 testData[32] = {1, 2, 3};
    sendFileDataPdu(TEST_CHANNEL_ID_0, TEST_GROUND_EID, component.getLocalEidParam(), 700, 50, 32, testData,
                    Cfdp::Class::CLASS_1);
    component.doDispatch();

    // Verify RxSeekFailed event
    // Note: Additional events may be emitted due to transaction state changes after seek failure
    ASSERT_EVENTS_RxSeekFailed_SIZE(1);
}

void CfdpManagerTester::testRxFileRenameFailedEvent() {
    // RxFileRenameFailed emitted in r2RecvMd() when the Class-2 receiver learns the true
    // destination filename from a late Metadata PDU and the rename (moveFile) of the
    // current/temp file to that final destination fails.
    //
    // r2RecvMd() logic (TransactionRx.cpp ~1013-1090):
    //   fname = m_history->fnames.dst_filename;         // saved BEFORE parsing -> move SOURCE
    //   engine->recvMd(...)  ->  m_history->fnames.dst_filename = md.getDestFilename();  // move DEST
    //   m_fd.close();
    //   moveFile(fname /*src*/, dst_filename /*dst*/);   // fails -> RxFileRenameFailed
    //
    // Strategy: the move SOURCE (the txn's initial dst_filename) must be a real file on disk,
    // while the Metadata PDU's destination filename points to a nonexistent directory so
    // moveFile() fails. eof_recv is left false so the EOF/MD size-mismatch check is skipped
    // and control reaches the rename block with success==true.

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::FileSize fileSize = 1000;
    Cfdp::TransactionSeq seq = 1;

    // Move SOURCE: the transaction's initial dst_filename (must exist on disk before moveFile)
    const char* moveSource = "test/ut/output/rename_src.bin";
    // Move DEST: comes from the Metadata PDU; point at a nonexistent directory to force failure
    const char* badDest = "/nonexistent_dir/subdir/renamed.bin";

    // Create the move-source file on disk so moveFile()'s source is valid
    Os::File srcFile;
    Os::File::Status openStatus = srcFile.open(moveSource, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, openStatus);
    U8 data[4] = {1, 2, 3, 4};
    FwSizeType writeSize = sizeof(data);
    srcFile.write(data, writeSize);
    srcFile.close();

    // Set up Class 2 receiver transaction; its dst_filename is the move SOURCE
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_R2,  // Class 2 receiver
                                            channelId,
                                            "test_src.dat",  // srcFilename (unused for rename)
                                            moveSource,      // dstFilename -> becomes the move SOURCE (fname)
                                            fileSize, seq, sourceEid);

    txn->m_engine = this->component.m_engine;
    txn->m_history->src_eid = sourceEid;

    // Force the "metadata arriving late" rename path:
    txn->m_flags.rx.md_recv = false;   // metadata not yet received -> enter rename branch
    txn->m_flags.rx.eof_recv = false;  // skip EOF/MD size-mismatch check -> keep success==true

    // Build the Metadata PDU whose destination filename is the (bad) rename target
    Cfdp::MetadataPdu metadataPdu;
    metadataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                           sourceEid,                           // sourceEid
                           seq,                                 // transactionSeq
                           this->component.getLocalEidParam(),  // destEid
                           fileSize,                            // fileSize
                           "test_src.dat",                      // source filename
                           badDest,                             // dest filename -> move DEST (bad dir)
                           Cfdp::ChecksumType::CHECKSUM_TYPE_MODULAR,
                           1);  // closureRequested for Class 2

    // Serialize PDU to buffer (direct r2RecvMd call: no packet descriptor prefix)
    U8 tempBuffer[300];
    Fw::SerialBuffer sb(tempBuffer, sizeof(tempBuffer));
    metadataPdu.serializeTo(sb);
    Fw::Buffer pduBuffer(tempBuffer, sb.getSize());

    this->clearEvents();

    // Directly call r2RecvMd -> parses MD (MetadataReceived) then moveFile fails (RxFileRenameFailed)
    txn->r2RecvMd(pduBuffer);

    // Verify events: MetadataReceived (from recvMd) + RxFileRenameFailed (rename failure)
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_MetadataReceived_SIZE(1);
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(1);

    // Cleanup: moveFile failed so the source file still exists
    Os::FileSystem::removeFile(moveSource);
}

void CfdpManagerTester::testTxFileSeekFailedEvent() {
    // TxFileSeekFailed emitted in sSubstateSendMetadata() (TransactionTx.cpp) when
    // m_fd.size() returns non-OP_OK after the file has been opened for reading:
    //     if (false == m_fd.isOpen()) {
    //         m_fd.open(src_filename, OPEN_READ);           // must SUCCEED
    //         ...
    //         fileStatus = m_fd.size(file_size);            // must FAIL -> TxFileSeekFailed
    //     }
    //
    // White-box trigger (real Posix Os backend, verified against Os/Posix source and
    // empirically on this Linux host):
    //   - Os::File::open(path, OPEN_READ) -> ::open(path, O_RDONLY). Opening a "special"
    //     seekable-less file such as /proc/cpuinfo SUCCEEDS (returns a valid fd).
    //   - Os::File::size() -> position() then ::lseek(fd, 0, SEEK_END). On a /proc file the
    //     initial ::lseek(fd, 0, SEEK_CUR) in position() succeeds (returns 0), but
    //     ::lseek(fd, 0, SEEK_END) FAILS with EINVAL, which errno_to_file_status() maps to
    //     INVALID_ARGUMENT (non-OP_OK) -> TxFileSeekFailed is emitted.
    //
    // Because success==false the code then calls setTxnStatus(FILESTORE_REJECTION) +
    // finishTransaction(), which also emits TxFileTransferFailed. handleNotKeepFile() takes
    // the TX-error branch and only acts on polling-dir files, so /proc/cpuinfo is left
    // untouched. We therefore assert with the specific _SIZE(1) macro, not ASSERT_EVENTS_SIZE.

    U8 channelId = 0;
    // /proc/cpuinfo: opens O_RDONLY successfully, but lseek(SEEK_END) fails EINVAL.
    const char* srcFile = "/proc/cpuinfo";
    const char* dstFile = "/ground/tx_seek.bin";
    U32 fileSize = 500;
    U32 sequenceId = 3200;
    U32 localEid = this->component.getLocalEidParam();

    this->clearHistory();
    this->clearEvents();

    // Create a Class 2 TX transaction (S2 sender state)
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            channelId, srcFile, dstFile, fileSize, sequenceId, TEST_GROUND_EID);
    ASSERT_NE(txn, nullptr) << "Transaction should be created";

    // Wire engine pointer (required for setTxnStatus / finishTransaction on failure)
    txn->m_engine = this->component.m_engine;

    // Set source entity ID (setupTestTransaction doesn't initialize this)
    txn->m_history->src_eid = localEid;

    // Drive the metadata substate so sSubstateSendMetadata() opens the file and calls size()
    txn->m_state_data.send.sub_state = TxSubState::TX_SUB_STATE_METADATA;

    // Ensure the file is not already open so sSubstateSendMetadata() opens it itself
    // (which is where the open(OPEN_READ)+size() sequence lives).
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }

    this->clearEvents();

    // Direct substate call: no async message is queued, so do NOT call doDispatch().
    txn->sSubstateSendMetadata();

    // Verify TxFileSeekFailed was emitted (TxFileTransferFailed also fires from
    // finishTransaction, so use the specific _SIZE(1) macro).
    ASSERT_EVENTS_TxFileSeekFailed_SIZE(1);

    // Verify event parameters (status is OS-dependent, so only check class/eid/seq).
    ASSERT_EQ(this->eventHistory_TxFileSeekFailed->at(0).cfdpClass, Cfdp::Class::CLASS_2);
    ASSERT_EQ(this->eventHistory_TxFileSeekFailed->at(0).srcEid, localEid);
    ASSERT_EQ(this->eventHistory_TxFileSeekFailed->at(0).seqNum, sequenceId);
}

void CfdpManagerTester::testTxSendMetadataFailedEvent() {
    // TxSendMetadataFailed is emitted by sSubstateSendMetadata() only when sendMd() returns
    // ERROR. That status is unreachable for a well-formed MetadataPdu (its
    // getBufferSize() matches toSerialBuffer() byte-for-byte, and buffer exhaustion yields the
    // retry status SEND_PDU_NO_BUF_AVAIL_ERROR, not ERROR). We therefore substitute a
    // SendMdFailEngine whose sendMd() override returns ERROR, driving the real substate through
    // the failure guard. Because success==false, the substate then runs setTxnStatus(
    // FILESTORE_REJECTION) + finishTransaction(), which additionally emits TxFileTransferFailed.

    const char* srcFile = "test/ut/output/tx_send_md_failed.bin";
    const char* dstFile = "/ground/tx_send_md_failed.bin";
    U32 fileSize = 3;
    U32 sequenceId = 4200;
    U32 localEid = this->component.getLocalEidParam();

    // Create a real, non-empty source file so sSubstateSendMetadata() opens it and reaches
    // sendMd() (a missing/zero-length file would divert to TxFileOpenFailed/TxZeroLengthFile).
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    U8 testData[3] = {1, 2, 3};
    FwSizeType sizeToWrite = 3;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearHistory();
    this->clearEvents();

    // Class 1 TX transaction (S1 sender state).
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S1, TEST_CHANNEL_ID_0, srcFile, dstFile, fileSize,
                                            sequenceId, TEST_GROUND_EID);
    ASSERT_NE(txn, nullptr) << "Transaction should be created";

    // Set source entity ID (setupTestTransaction doesn't initialize this).
    txn->m_history->src_eid = localEid;

    // Keep the source file so the post-event finishTransaction() -> handleNotKeepFile() does
    // not delete/move it (which could emit FileRemoveFailed/FailKeepFileMove).
    txn->m_keep = Keep::KEEP;

    // Substitute an engine whose sendMd() forces the failure branch. It borrows the real
    // allocator so ~Engine()'s FW_ASSERT(m_allocator != nullptr) is satisfied; all of its
    // m_channels are null, so the destructor cleanup loop is a no-op. m_chan must point at a
    // real channel because finishTransaction() dereferences it.
    SendMdFailEngine failEngine(&this->component);
    failEngine.m_allocator = this->component.m_engine->m_allocator;
    txn->m_engine = &failEngine;
    txn->m_chan = this->component.m_engine->m_channels[TEST_CHANNEL_ID_0];

    // Drive the metadata substate.
    txn->m_state_data.send.sub_state = TxSubState::TX_SUB_STATE_METADATA;
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }

    this->clearEvents();

    // Direct substate call: no async message is queued, so do NOT call doDispatch().
    txn->sSubstateSendMetadata();

    // Restore the real engine before the transaction pool slot can be reused.
    txn->m_engine = this->component.m_engine;

    // Verify events: TxSendMetadataFailed from the guard, plus TxFileTransferFailed from the
    // finishTransaction() failure tail.
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_TxSendMetadataFailed_SIZE(1);
    ASSERT_EVENTS_TxFileTransferFailed_SIZE(1);

    // Verify TxSendMetadataFailed parameters.
    ASSERT_EVENTS_TxSendMetadataFailed(0,                     // index
                                       Cfdp::Class::CLASS_1,  // cfdpClass
                                       localEid,              // srcEid
                                       sequenceId             // seqNum
    );

    // Cleanup.
    Os::FileSystem::removeFile(srcFile);
}

// ----------------------------------------------------------------------
// Informational/Diagnostic Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testRxTempFileCreatedEvent() {
    // RxTempFileCreated emitted when temporary file created for Class 2 RX without metadata

    this->clearEvents();

    // Send FileData PDU before Metadata PDU to trigger temp file creation
    U8 testData[32] = {1, 2, 3};
    sendFileDataPdu(TEST_CHANNEL_ID_0, TEST_GROUND_EID, component.getLocalEidParam(), 1000, 0, 32, testData,
                    Cfdp::Class::CLASS_2);
    component.doDispatch();

    // Verify RxTempFileCreated event
    ASSERT_EVENTS_RxTempFileCreated_SIZE(1);

    // Cleanup temp file (temp file format: <tmpDir>/<src_eid>:<seq_num>.tmp)
    // Will be cleaned up by transaction cleanup
}

void CfdpManagerTester::testDanglingFileHandleClosedEvent() {
    // DanglingFileHandleClosed emitted when a transaction is recycled with its file still
    // open. The event is emitted by Channel::recycleTransaction() (Channel.cpp), NOT by
    // freeTransaction() - freeTransaction() calls txn->reset() which silently closes the
    // file, so it can never observe a dangling handle. recycleTransaction() checks
    // m_fd.isOpen() up front (before the reset in freeTransaction) and logs the event.
    //
    // We use a REAL RX transaction (established via a Metadata PDU) rather than the
    // white-box setupTestTransaction() helper, because recycleTransaction() ->
    // dequeueTransaction() removes the transaction from its queue by q_index and adjusts
    // queue-depth telemetry. Only a genuinely-queued transaction has a valid q_index
    // (QueueId::RX) and allocated chunk list, so the recycle path executes safely.

    const char* testFile = "test/ut/output/dangling.bin";
    U8 channelId = TEST_CHANNEL_ID_0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1100;
    Cfdp::FileSize fileSize = 100;

    // Create a real file the transaction can hold open
    Os::File file;
    Os::File::Status fileStatus = file.open(testFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    U8 data[10] = {1, 2, 3};
    FwSizeType size = 3;
    fileStatus = file.write(data, size);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    file.close();

    this->clearHistory();
    this->clearEvents();

    // Establish a real RX transaction (placed on QueueId::RX with a valid chunk list)
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, "/ground/dangling_src.bin", testFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "RX transaction should exist";

    // Force a dangling file handle: open the transaction's file and leave it open
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }
    fileStatus = txn->m_fd.open(testFile, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    ASSERT_TRUE(txn->m_fd.isOpen());

    this->clearEvents();

    // Recycle the transaction with the file still open -> DanglingFileHandleClosed
    Channel* chan = component.m_engine->m_channels[channelId];
    chan->recycleTransaction(txn);

    // Verify DanglingFileHandleClosed event
    ASSERT_EVENTS_DanglingFileHandleClosed_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(testFile);
}

void CfdpManagerTester::testResetFreedTransactionEvent() {
    // ResetFreedTransaction (DIAGNOSTIC) is emitted when finishTransaction() is called on a
    // transaction that has already been freed. The double-free guard in
    // Engine::finishTransaction() keys off q_index == QueueId::FREE, and
    // Channel::freeTransaction() now tags freed transactions with that queue id.

    // Acquire a transaction and put it into a live sender state.
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S1, TEST_CHANNEL_ID_0, "/test/src.bin", "/test/dst.bin",
                                            100, 1200, TEST_GROUND_EID);

    // Free the transaction back onto the FREE list. After the fix this tags q_index == FREE.
    Channel* chan = component.m_engine->m_channels[TEST_CHANNEL_ID_0];
    chan->freeTransaction(txn);
    ASSERT_EQ(QueueId::FREE, txn->m_flags.com.q_index);

    this->clearEvents();

    // Calling finishTransaction() on the already-freed transaction must trip the double-free
    // guard and emit exactly one ResetFreedTransaction diagnostic (and nothing else).
    component.m_engine->finishTransaction(txn, false);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ResetFreedTransaction_SIZE(1);
}

// ----------------------------------------------------------------------
// RX CRC and Validation Events
// ----------------------------------------------------------------------

void CfdpManagerTester::testRxSeekCrcFailedEvent() {
    // RxSeekCrcFailed emitted when seek fails during CRC calculation
    // Strategy: Use white-box access to set up transaction in CRC calculation state,
    // then delete file to cause seek failure

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::TransactionSeq transactionSeq = 2700;
    const char* srcFile = "/ground/crc_seek_fail.bin";
    const char* dstFile = "test/ut/output/crc_seek_fail.bin";

    // File must be larger than RxCrcCalcBytesPerCycle (default 65536) to require multiple CRC ticks
    Cfdp::FileSize fileSize = 70000;

    this->clearHistory();
    this->clearEvents();

    // Create test file
    Os::File file;
    Os::File::Status fileStatus = file.open(dstFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    U8 testData[1000];
    memset(testData, 0xAB, sizeof(testData));
    for (U32 i = 0; i < fileSize / sizeof(testData); i++) {
        FwSizeType size = sizeof(testData);
        fileStatus = file.write(testData, size);
        ASSERT_EQ(Os::File::OP_OK, fileStatus);
    }
    file.close();

    // Set up a Class 2 RX transaction using setupTestTransaction helper
    Transaction* txn =
        setupTestTransaction(TxnState::TXN_STATE_R2, channelId, srcFile, dstFile, fileSize, transactionSeq, sourceEid);
    this->clearEvents();
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Set up state for CRC calculation:
    // - Set send_fin flag to true (triggers CRC calculation in r2SubstateSendFin)
    // - Set rx_crc_calc_bytes to non-zero to simulate partial CRC progress
    // - File must exist and be openable
    txn->m_flags.rx.send_fin = true;
    txn->m_flags.rx.md_recv = true;
    txn->m_flags.rx.eof_recv = true;
    txn->m_fsize = fileSize;
    txn->m_state_data.receive.r2.rx_crc_calc_bytes = 65536;  // Simulate first CRC tick completed

    // Close any existing file handle (transaction may have opened for metadata processing)
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }

    // Open the file in READ mode to simulate CRC calculation state
    fileStatus = txn->m_fd.open(dstFile, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    txn->m_state_data.receive.cached_pos = 0;  // Position mismatch will trigger seek

    // Close the file descriptor to make seek fail
    // (On Unix, deleting an open file keeps the fd valid, so we close it first)
    txn->m_fd.close();

    // Call r2CalcCrcChunk directly - this will try to seek on closed fd and fail
    Cfdp::Status::T status = txn->r2CalcCrcChunk();

    // Verify seek failure was detected
    ASSERT_EQ(Cfdp::Status::ERROR, status);

    // Verify RxSeekCrcFailed event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_RxSeekCrcFailed_SIZE(1);
    ASSERT_EVENTS_RxSeekCrcFailed(0, Cfdp::Class::CLASS_2,
                                  txn->m_history->src_eid,  // Use actual src_eid from transaction
                                  transactionSeq,
                                  65536,                        // The offset where seek failed
                                  Os::File::Status::NOT_OPENED  // Closed file descriptor
    );

    // Cleanup
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testRxReadCrcFailedEvent() {
    // RxReadCrcFailed emitted when read fails during CRC calculation
    // This test verifies that read failures during CRC calculation are properly detected and reported
    const U32 channelId = 0;
    const U32 transactionSeq = 42;
    const U32 sourceEid = 100;
    const char* srcFile = "source_file.bin";
    const char* dstFile = "test/ut/output/crc_read_fail.bin";

    // File must be larger than RxCrcCalcBytesPerCycle (default 65536) to require multiple CRC ticks
    Cfdp::FileSize fileSize = 131072;  // 128KB

    this->clearHistory();
    this->clearEvents();

    // Create test file
    Os::File file;
    Os::File::Status fileStatus = file.open(dstFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    U8 testData[1000];
    memset(testData, 0xCD, sizeof(testData));
    for (U32 i = 0; i < fileSize / sizeof(testData); i++) {
        FwSizeType size = sizeof(testData);
        fileStatus = file.write(testData, size);
        ASSERT_EQ(Os::File::OP_OK, fileStatus);
    }
    file.close();

    // Set up a Class 2 RX transaction using setupTestTransaction helper
    Transaction* txn =
        setupTestTransaction(TxnState::TXN_STATE_R2, channelId, srcFile, dstFile, fileSize, transactionSeq, sourceEid);
    this->clearEvents();
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Set up state for CRC calculation:
    // - Set send_fin flag to true (triggers CRC calculation in r2SubstateSendFin)
    // - Set rx_crc_calc_bytes to non-zero to simulate partial CRC progress
    // - Set cached_pos equal to rx_crc_calc_bytes to skip seek and proceed to read
    txn->m_flags.rx.send_fin = true;
    txn->m_flags.rx.md_recv = true;
    txn->m_flags.rx.eof_recv = true;
    txn->m_fsize = fileSize;
    txn->m_state_data.receive.r2.rx_crc_calc_bytes = 65536;  // Simulate first CRC tick completed
    txn->m_state_data.receive.cached_pos = 65536;            // Match rx_crc_calc_bytes to skip seek

    // Close any existing file handle (transaction may have opened for metadata processing)
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }

    // Open the file in READ mode to simulate CRC calculation state
    fileStatus = txn->m_fd.open(dstFile, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);

    // Close the file descriptor to make read fail
    // Since cached_pos == rx_crc_calc_bytes, seek will be skipped and read will execute on closed fd
    txn->m_fd.close();

    // Call r2CalcCrcChunk directly - this will try to read on closed fd and fail
    Cfdp::Status::T status = txn->r2CalcCrcChunk();

    // Verify read failure was detected
    ASSERT_EQ(Cfdp::Status::ERROR, status);

    // Verify RxReadCrcFailed event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_RxReadCrcFailed_SIZE(1);
    ASSERT_EVENTS_RxReadCrcFailed(0, Cfdp::Class::CLASS_2,
                                  txn->m_history->src_eid,  // Use actual src_eid from transaction
                                  transactionSeq,
                                  CFDP_R2_CRC_CHUNK_SIZE,  // Expected read size (CRC chunk buffer size, 1024)
                                  0                        // Actual read size (read failed, returns 0)
    );

    // Cleanup
    if (txn->m_fd.isOpen()) {
        txn->m_fd.close();
    }
    Os::FileSystem::removeFile(dstFile);
}

// ----------------------------------------------------------------------
// Port and Command Input Validation
// ----------------------------------------------------------------------

void CfdpManagerTester::testUnsupportedSendFileArgumentsEvent() {
    // UnsupportedSendFileArguments emitted when fileIn port called with non-zero offset/length

    this->clearEvents();

    // Create a simple test file first
    const char* testFile = "test/ut/output/unsupported_args_test.bin";
    Os::File file;
    file.open(testFile, Os::File::OPEN_CREATE);
    U8 data[10] = {1, 2, 3};
    FwSizeType size = 3;
    file.write(data, size);
    file.close();

    // Invoke fileIn with non-zero offset (unsupported)
    Fw::String source(testFile);
    Fw::String dest("/dest/test.bin");

    // Call with offset=100, length=50 (unsupported)
    // fileIn is a guarded (synchronous) port: the handler runs inline and emits the
    // event immediately. Do NOT call doDispatch() here - it would block forever on the
    // active component's empty message queue.
    invoke_to_fileIn(0, source, dest, 100, 50);

    // Verify UnsupportedSendFileArguments event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UnsupportedSendFileArguments_SIZE(1);

    // Cleanup
    Os::FileSystem::removeFile(testFile);
}

void CfdpManagerTester::testSendFileInitiateFailEvent() {
    // SendFileInitiateFail emitted when engine txFile() returns error.
    //
    // txFile() does NOT validate that the source file exists (a nonexistent file is
    // detected later, during a TX tick, as TxFileOpenFailed). The only path that makes
    // txFile() return ERROR is transaction-slot exhaustion, so drive that: fill the
    // channel's commanded-TX slots, then invoke the fileIn port. The port handler maps
    // the txFile error to SendFileInitiateFail (CfdpManager.cpp:166).

    // Fill the commanded-TX slots on the default fileIn channel so the next txFile() fails.
    Fw::ParamValid valid;
    U8 channelId = this->component.paramGet_FileInDefaultChannel(valid);
    Channel* chan = this->component.m_engine->m_channels[channelId];
    FW_ASSERT(chan != nullptr);
    chan->m_numCmdTx = CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN;

    this->clearEvents();

    Fw::String source("test/ut/output/send_file_initiate_fail.bin");
    Fw::String dest("/dest/test.bin");

    // fileIn is a guarded (synchronous) port: the handler runs inline and emits the
    // event immediately. Do NOT call doDispatch() here - it would block forever on the
    // active component's empty message queue.
    invoke_to_fileIn(0, source, dest, 0, 0);

    // Verify SendFileInitiateFail event
    ASSERT_EVENTS_SendFileInitiateFail_SIZE(1);
}

void CfdpManagerTester::testInvalidChannelPollEvent() {
    // InvalidChannelPoll emitted when poll command with invalid index

    this->clearEvents();

    // Send PollDirectory with pollIndex out of range
    U8 invalidPollIndex = 255;  // Guaranteed to be >= CFDP_MAX_POLLING_DIR_PER_CHAN
    sendCmd_PollDirectory(0, 0, TEST_CHANNEL_ID_0, invalidPollIndex, TEST_GROUND_EID, Cfdp::Class::CLASS_1, 0, 10,
                          Fw::CmdStringArg("test/ut/output"), Fw::CmdStringArg("/dest"));
    component.doDispatch();

    // Verify InvalidChannelPoll event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannelPoll_SIZE(1);
}

void CfdpManagerTester::testChunklistUnavailableEvent() {
    // ChunklistUnavailable emitted when all RX chunklists exhausted but transaction slots remain
    // Strategy: Use white-box access to manually exhaust all RX chunklists by allocating them
    // directly, then send a FileData PDU which will get a free transaction but fail at chunklist allocation

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();

    this->clearHistory();
    this->clearEvents();

    // White-box: Get direct access to channel
    Channel* chan = this->component.m_engine->m_channels[channelId];
    ASSERT_NE(chan, nullptr);

    // White-box: Manually exhaust all RX chunklists by allocating them directly
    // There are CFDP_NUM_TRANSACTIONS_PER_CHANNEL RX chunklists per channel
    // We allocate them but don't use them, leaving transaction slots available
    CfdpChunkWrapper* exhaustedChunks[CFDP_NUM_TRANSACTIONS_PER_CHANNEL];
    for (U32 i = 0; i < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; i++) {
        exhaustedChunks[i] = chan->findUnusedChunks(Direction::DIRECTION_RX);
        ASSERT_NE(exhaustedChunks[i], nullptr) << "Failed to allocate RX chunklist " << i;
    }

    // Verify no more RX chunklists available
    CfdpChunkWrapper* shouldBeNull = chan->findUnusedChunks(Direction::DIRECTION_RX);
    ASSERT_EQ(shouldBeNull, nullptr);

    this->clearEvents();

    // Send FileData PDU for a new transaction
    // Engine::receivePdu() will call startRxTransaction() which succeeds (transaction slots available)
    // Engine::recvInit() will attempt to allocate RX chunklist at line 506 but fail
    // ChunklistUnavailable event emitted at line 509
    U8 testData[] = {0x01, 0x02, 0x03, 0x04};
    this->sendFileDataPdu(channelId, sourceEid, destEid,
                          9999,  // New transaction sequence
                          0,     // offset
                          sizeof(testData), testData, Cfdp::Class::CLASS_2);
    this->component.doDispatch();

    // Verify ChunklistUnavailable event
    // Note: Two events are emitted:
    // 1. ChunklistUnavailable (line 509 of Engine.cpp) - when chunklist allocation fails
    // 2. RxFileTransferCompleted (from finishTransaction line 962) - transaction status is UNDEFINED
    //    which TxnStatusIsError treats as non-error, so success path is taken
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_ChunklistUnavailable_SIZE(1);
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);

    // Note: No cleanup needed - chunklists remain allocated but test isolation
    // handles component teardown
}

void CfdpManagerTester::testFailKeepFileMoveEvent() {
    // FailKeepFileMove emitted when file move to keep directory fails after TX
    // Strategy: Create TX transaction via setupTestTransaction, set invalid move directory,
    // call handleNotKeepFile()

    U8 channelId = 0;
    const char* srcFile = "test/ut/output/move_test_src.bin";
    const char* dstFile = "/ground/move_test_dst.bin";

    // Step 1: Create source file for TX transaction
    // Tests run from fprime root, so use Svc-relative path
    Os::File testFile;
    Os::File::Status fileOpenStatus = testFile.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileOpenStatus) << "Failed to create test file " << srcFile;

    U8 testData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    FwSizeType sizeWritten = 0;
    Os::File::Status fileWriteStatus = testFile.write(testData, sizeWritten, Os::File::WAIT);
    ASSERT_EQ(Os::File::OP_OK, fileWriteStatus);
    testFile.close();

    // Step 2: Setup TX transaction using white-box helper
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S1,  // Class 1 sender
                                            channelId, srcFile, dstFile,
                                            10,              // File size
                                            4000,            // Transaction sequence
                                            TEST_GROUND_EID  // Peer
    );

    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Step 3: Configure transaction internals for handleNotKeepFile
    txn->m_engine = component.m_engine;
    txn->m_chan = component.m_engine->m_channels[channelId];
    txn->m_keep = Cfdp::Keep::DELETE;  // Must be DELETE to trigger handleNotKeepFile logic
    // Note: txn_stat doesn't need explicit setting - TxnStatusIsError checks will pass
    // for default state

    // Step 4: Try to set move directory parameter using paramSet
    Cfdp::ChannelArrayParams channelConfig;

    // Initialize channel 0 parameters with defaults
    Cfdp::ChannelParams& params = channelConfig[channelId];
    params.set_ack_limit(2);
    params.set_nack_limit(3);
    params.set_ack_timer(5000);
    params.set_inactivity_timer(30000);
    params.set_dequeue_enabled(Fw::Enabled::ENABLED);

    // Use Fw::String for string parameters
    Fw::String invalidMoveDir("/nonexistent/protected/move/directory");
    Fw::String tmpDir("test/ut/output/tmp");
    Fw::String failDir("test/ut/output/fail");

    params.set_move_dir(invalidMoveDir);
    params.set_max_outgoing_pdus_per_cycle(10);
    params.set_tmp_dir(tmpDir);
    params.set_fail_dir(failDir);

    // Stage the parameter value in the tester, then send it to the component.
    // paramSet_ only stages the value in the tester; paramSend_ invokes the
    // component's PRM_SET command handler over the cmdIn port. Parameter SET
    // opcodes are dispatched SYNCHRONOUSLY in cmdIn_handlerBase (unlike async
    // commands such as SendFile, which queue for doDispatch()), so the
    // component's OWN parameter storage (m_ChannelConfig) is updated inline.
    // No doDispatch() is needed here -- calling it would block on an empty
    // message queue.
    this->paramSet_ChannelConfig(channelConfig, Fw::ParamValid::VALID);
    this->paramSend_ChannelConfig(0, 0);

    // Verify move directory propagated to the component's own storage.
    Fw::String moveDir = component.getMoveDirParam(channelId);
    ASSERT_GT(moveDir.length(), 0u)
        << "move_dir parameter should have propagated to the component via paramSend + doDispatch";

    // Step 5: Clear event history and call handleNotKeepFile directly
    this->clearEvents();

    component.m_engine->handleNotKeepFile(txn);

    // Step 6: Assert FailKeepFileMove event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailKeepFileMove_SIZE(1);

    // Step 7: Cleanup test file
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testFailPduSerializationEvent() {
    // FailPduSerialization emitted when PDU serialization fails
    // Strategy: Create PDU that reports smaller size than it actually needs

    // Setup: Create minimal Class 2 sender transaction
    const U32 testSequenceId = 42;
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S2,  // Class 2 sender
                                            TEST_CHANNEL_ID_0,       // Channel 0
                                            "src.txt",               // Source file
                                            "dst.txt",               // Dest file
                                            1000,                    // File size
                                            testSequenceId,
                                            TEST_GROUND_EID  // Peer entity ID
    );
    ASSERT_NE(txn, nullptr);

    // Create test PDU that lies about its size
    // Report 20 bytes but actually try to write 100 bytes
    // This simulates a bug in PDU size calculation
    const U32 reportedSize = 20;
    const U32 actualSize = 100;
    OversizedTestPdu oversizedPdu(reportedSize, actualSize);

    // Clear event history before test
    this->clearEvents();

    // Execute: Call serializeAndSendPdu
    // - getPduBuffer will allocate 20 bytes (based on reportedSize)
    // - serializeTo will try to write 100 bytes
    // - Serialization will fail with FW_SERIALIZE_NO_ROOM_LEFT
    // - FailPduSerialization event should be emitted
    Cfdp::Status::T status = component.m_engine->serializeAndSendPdu(txn, oversizedPdu);

    // Verify: Serialization should fail
    ASSERT_EQ(Cfdp::Status::ERROR, status);

    // Verify: FailPduSerialization event was emitted with correct parameters
    ASSERT_EVENTS_FailPduSerialization_SIZE(1);
    ASSERT_EVENTS_FailPduSerialization(0,                                               // Event index
                                       TEST_CHANNEL_ID_0,                               // Channel ID
                                       Cfdp::PduTypeEnum::ACKNOWLEDGMENT,               // PDU type
                                       static_cast<I32>(Fw::FW_SERIALIZE_NO_ROOM_LEFT)  // Serialize status
    );
}

void CfdpManagerTester::testFailPollFileMoveEvent() {
    // FailPollFileMove emitted when a source file that lives in a registered polling
    // directory fails to move to the channel's fail directory after an errored TX.
    //
    // Path exercised in Engine::handleNotKeepFile():
    //   dir == DIRECTION_TX && TxnStatusIsError(txn_stat) && isPollingDir(src, chan)
    //   -> failDir = getFailDirParam(chan); if (failDir.length() > 0)
    //      -> Os::FileSystem::moveFile(src, failDir) fails -> log_WARNING_LO_FailPollFileMove
    //
    // isPollingDir() compares the directory portion of the source filename (everything
    // before the last '/') against each channel PollDir::srcDir slot. We register a
    // slot by writing directly to the channel's CfdpPollDir via getPollDir() (friend
    // access), setting srcDir to the exact directory portion of our source file.

    U8 channelId = 0;
    const char* srcDirPath = "test/ut/output";
    const char* srcFile = "test/ut/output/poll_move_test.bin";
    const char* dstFile = "/ground/poll_move_dst.bin";

    this->clearHistory();
    this->clearEvents();

    // Step 1: Create a real source file (CWD is the component dir)
    Os::File testFile;
    Os::File::Status fileOpenStatus = testFile.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileOpenStatus) << "Failed to create test file " << srcFile;
    U8 testData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    FwSizeType sizeWritten = sizeof(testData);
    Os::File::Status fileWriteStatus = testFile.write(testData, sizeWritten, Os::File::WAIT);
    ASSERT_EQ(Os::File::OP_OK, fileWriteStatus);
    testFile.close();

    // Step 2: Set up a TX transaction (Class 1 sender)
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_S1,  // Class 1 sender
                                            channelId, srcFile, dstFile,
                                            10,              // File size
                                            4100,            // Transaction sequence
                                            TEST_GROUND_EID  // Peer
    );
    ASSERT_TRUE(txn != nullptr) << "Transaction should exist";

    // Step 3: Configure transaction internals for handleNotKeepFile
    txn->m_engine = component.m_engine;
    txn->m_chan = component.m_engine->m_channels[channelId];
    txn->m_keep = Cfdp::Keep::DELETE;  // DELETE selects the move/delete path
    // setupTestTransaction already sets history->dir = DIRECTION_TX; assert to be explicit
    ASSERT_EQ(Direction::DIRECTION_TX, txn->m_history->dir);

    // Step 4: Put the transaction into an error state so TxnStatusIsError() is true.
    // TxnStatusIsError returns (txn_stat > TXN_STATUS_NO_ERROR); PROTOCOL_ERROR (16) qualifies.
    txn->m_history->txn_stat = TxnStatus::TXN_STATUS_PROTOCOL_ERROR;

    // Step 5: Register a polling-dir slot so isPollingDir(src, chan) matches.
    // Write srcDir directly on the channel's first CfdpPollDir slot (friend access).
    Channel* chan = component.m_engine->m_channels[channelId];
    ASSERT_NE(chan, nullptr);
    CfdpPollDir* pd = chan->getPollDir(0);
    ASSERT_NE(pd, nullptr);
    pd->srcDir = Fw::String(srcDirPath);  // exact directory portion of srcFile
    ASSERT_TRUE(component.m_engine->isPollingDir(txn->m_history->fnames.src_filename, channelId))
        << "isPollingDir should match the registered polling-dir slot";

    // Step 6: The fail_dir parameter default ("/fail") is non-empty; the target directory
    // does not exist so Os::FileSystem::moveFile() will fail, triggering the event.
    Fw::String failDir = component.getFailDirParam(channelId);
    ASSERT_GT(failDir.length(), 0u) << "fail_dir default must be non-empty";

    this->clearEvents();

    // Step 7: Direct engine call (nothing queued -> no doDispatch)
    component.m_engine->handleNotKeepFile(txn);

    // Step 8: Assert exactly the FailPollFileMove event was emitted.
    // (moveFile status is OS-dependent, so only the count is asserted.)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FailPollFileMove_SIZE(1);

    // Cleanup: the move failed so the source file remains
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testFileDataSegmentMetadataEvent() {
    // FileDataSegmentMetadata emitted when a received FileData PDU has the (unsupported)
    // segment-metadata flag set. Engine::recvFd() checks header.hasSegmentMetadata() and,
    // if set, logs FileDataSegmentMetadata, marks the transaction PROTOCOL_ERROR, increments
    // recv errors, and returns ERROR.
    //
    // There is no public setter for the segment-metadata flag and the tester is not a friend
    // of PduHeader/FileDataPdu. Strategy (direct recvFd call):
    //   1. Establish a real RX transaction via a Metadata PDU so recvFd has a valid txn.
    //   2. Build a valid FileDataPdu and serialize it to a raw buffer (no packet-descriptor
    //      prefix - the descriptor is only used on the dataIn path).
    //   3. Flip bit 3 (0x08) of the header's eidTsnLengths byte. In the serialized PDU the
    //      fixed header layout is: byte 0 = flags, bytes 1-2 = PDU data length, byte 3 =
    //      eidTsnLengths (see Types/PduHeader.cpp serialize routine). So the segment-metadata
    //      bit lives at absolute offset 3.
    //   4. Deserialize the modified buffer back into a fresh FileDataPdu (header now reports
    //      hasSegmentMetadata() == true; body still parses because the flag consumes no bytes).
    //   5. Call component.m_engine->recvFd(txn, fd2) and assert the event.

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::EntityId destEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 2800;
    const char* srcFile = "/ground/segmeta_test.bin";
    const char* dstFile = "test/ut/output/segmeta_test.bin";
    Cfdp::FileSize fileSize = 100;

    this->clearHistory();
    this->clearEvents();

    // Step 1: Establish an RX transaction with a Metadata PDU
    this->sendMetadataPdu(channelId, sourceEid, destEid, transactionSeq, fileSize, srcFile, dstFile,
                          Cfdp::Class::CLASS_1, 0);
    this->component.doDispatch();

    Transaction* txn = this->findTransaction(channelId, transactionSeq);
    ASSERT_TRUE(txn != nullptr) << "RX transaction should exist";

    // Step 2: Build a valid FileData PDU
    Cfdp::FileDataPdu fileDataPdu;
    U8 testData[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    fileDataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_1, sourceEid,
                           transactionSeq, destEid, 0, 10, testData);  // offset, dataSize, data

    // Step 3: Serialize it into a raw buffer (NO packet-descriptor prefix)
    U32 pduSize = fileDataPdu.getBufferSize();
    U8 rawBuffer[300];
    Fw::SerialBuffer sb(rawBuffer, sizeof(rawBuffer));
    Fw::SerializeStatus serStatus = fileDataPdu.serializeTo(sb);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serStatus);

    // Step 4: Set the segment-metadata flag (bit 3, 0x08) in the eidTsnLengths byte,
    // which is at absolute offset 3 within the serialized PDU.
    const U32 EID_TSN_LENGTHS_OFFSET = 3;
    rawBuffer[EID_TSN_LENGTHS_OFFSET] = static_cast<U8>(rawBuffer[EID_TSN_LENGTHS_OFFSET] | 0x08U);

    // Step 5: Deserialize the modified buffer into a fresh FileDataPdu.
    // setBuffLen marks the buffer as holding pduSize bytes of valid data so the
    // deserializer knows how much is available (same pattern used in Engine.cpp).
    Fw::SerialBuffer in(rawBuffer, sizeof(rawBuffer));
    in.setBuffLen(pduSize);

    Cfdp::FileDataPdu fd2;
    Fw::SerializeStatus deserStatus = fd2.deserializeFrom(in);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, deserStatus);

    // Sanity check: the flag round-tripped
    ASSERT_TRUE(fd2.asHeader().hasSegmentMetadata())
        << "Modified FileData PDU header should report segment metadata present";

    this->clearEvents();

    // Step 6: Call recvFd directly - should log the event and return ERROR
    Cfdp::Status::T st = this->component.m_engine->recvFd(txn, fd2);

    // Verify FileDataSegmentMetadata event was emitted and ERROR returned
    ASSERT_EVENTS_FileDataSegmentMetadata_SIZE(1);
    ASSERT_EQ(Cfdp::Status::ERROR, st);

    // Cleanup
    Os::FileSystem::removeFile(dstFile);
}

void CfdpManagerTester::testPlaybackDirReadFailedEvent() {
    // PlaybackDirReadFailed emitted when directory read fails during a playback scan.
    //
    // Strategy (white-box, no OS mocking): Initiate a REAL playback so a slot opens the
    // directory successfully (playbackDir -> playbackDirInitiate -> pb->dir.open()). Then
    // close the underlying directory handle (pb->dir.close()) WITHOUT clearing pb->diropen
    // or pb->busy. On the next 1Hz cycle the engine drives
    //   run1Hz -> Engine::cycle() -> Channel::processPlaybackDirectories()
    //          -> Channel::processPlaybackDirectory()
    // which, with pending_file empty, calls pb->dir.read(). Because m_is_open is now false,
    // Os::Directory::read() returns NOT_OPENED (!= OP_OK and != NO_MORE_FILES), so the code
    // logs PlaybackDirReadFailed and tears the playback down.

    U8 channelId = TEST_CHANNEL_ID_0;
    Fw::String srcDir("test/ut/output");  // real, existing directory (CWD = component dir)
    Fw::String dstDir("/dest");

    this->clearHistory();
    this->clearEvents();

    // Initiate a real playback: opens the directory successfully and marks the slot busy.
    Cfdp::Status::T status = this->component.m_engine->playbackDir(srcDir, dstDir, Cfdp::Class::CLASS_1,
                                                                   Cfdp::Keep::DELETE, channelId, 0, TEST_GROUND_EID);
    ASSERT_EQ(status, Cfdp::Status::SUCCESS) << "playbackDir should succeed and open the directory";

    // Locate the busy playback slot that just opened the directory (index 0 expected).
    Channel* chan = this->component.m_engine->m_channels[channelId];
    ASSERT_NE(chan, nullptr);

    Playback* pb = nullptr;
    for (U32 i = 0; i < CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i) {
        Playback* candidate = chan->getPlayback(i);
        if (candidate->busy && candidate->diropen) {
            pb = candidate;
            break;
        }
    }
    ASSERT_NE(pb, nullptr) << "An opened playback slot should exist";
    ASSERT_TRUE(pb->dir.isOpen()) << "Playback directory handle should be open";

    // White-box trick: close the directory handle so the next read() fails with NOT_OPENED,
    // but leave diropen/busy set so the scan still attempts the read.
    pb->dir.close();
    ASSERT_FALSE(pb->dir.isOpen());
    ASSERT_TRUE(pb->diropen) << "diropen must remain set so the scan attempts a read";

    this->clearEvents();

    // Drive the 1Hz cycle that runs processPlaybackDirectory -> pb->dir.read() (fails).
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Verify PlaybackDirReadFailed event was emitted.
    ASSERT_EVENTS_PlaybackDirReadFailed_SIZE(1);
}

void CfdpManagerTester::testPlaybackDirSlotUnavailableEvent() {
    // PlaybackDirSlotUnavailable emitted when all playback slots exhausted
    // Strategy: Fill all CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN
    // playback slots with successful playbackDir() calls (each consumes a slot
    // by opening a real directory), then attempt one more which must fail.

    U8 channelId = TEST_CHANNEL_ID_0;
    Fw::String srcDir("test/ut/output");  // real, existing directory (CWD = component dir)
    Fw::String dstDir("/dest");

    this->clearHistory();
    this->clearEvents();

    // Consume every playback slot on the channel. Each call opens the real
    // directory successfully, setting pb->busy = true for that slot.
    for (U32 i = 0; i < CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i) {
        Cfdp::Status::T setupStatus = this->component.m_engine->playbackDir(
            srcDir, dstDir, Cfdp::Class::CLASS_1, Cfdp::Keep::DELETE, channelId, 0, TEST_GROUND_EID);
        ASSERT_EQ(setupStatus, Cfdp::Status::SUCCESS) << "Setup playbackDir call " << i << " should consume a slot";
    }

    // Only assert on the event emitted by the final (failing) call.
    this->clearEvents();

    // All slots are now busy; this call must fail and emit the event.
    Cfdp::Status::T status = this->component.m_engine->playbackDir(srcDir, dstDir, Cfdp::Class::CLASS_1,
                                                                   Cfdp::Keep::DELETE, channelId, 0, TEST_GROUND_EID);

    ASSERT_EQ(status, Cfdp::Status::ERROR) << "playbackDir should fail when all slots are busy";
    ASSERT_EVENTS_PlaybackDirSlotUnavailable_SIZE(1);
}

void CfdpManagerTester::testRxFileReopenFailedEvent() {
    // RxFileReopenFailed emitted in r2RecvMd() (TransactionRx.cpp ~1073) on the SUCCESS branch
    // of the file rename: moveFile(fname -> dst_filename) succeeds, but the immediately-following
    // reopen open(dst_filename, OPEN_WRITE) fails.
    //
    // r2RecvMd() logic:
    //   fname = m_history->fnames.dst_filename;          // saved BEFORE parsing -> move SOURCE
    //   engine->recvMd(...) -> m_history->fnames.dst_filename = md.getDestFilename();  // move DEST
    //   m_fd.close();
    //   moveFile(fname /*src*/, dst_filename /*dst*/);    // must SUCCEED
    //   m_fd.open(dst_filename, OPEN_WRITE);              // must FAIL -> RxFileReopenFailed
    //
    // White-box trigger (real Posix Os backend, verified against Os/Posix source):
    //   - Os::FileSystem::moveFile() -> _rename() -> ::rename(2). rename(2) will happily rename a
    //     DIRECTORY to a new name, so if the move SOURCE (the txn's initial dst_filename) is a
    //     directory, the rename SUCCEEDS and the DEST path is now itself a directory.
    //   - Os::File::open(OPEN_WRITE) -> ::open(dst, O_WRONLY | O_CREAT). Opening an existing
    //     directory O_WRONLY fails with EISDIR, which errno_to_file_status() maps to a non-OP_OK
    //     status -> RxFileReopenFailed is emitted.
    //
    // eof_recv is left false so the EOF/MD size-mismatch check is skipped and control reaches the
    // rename block with success==true.

    U8 channelId = 0;
    Cfdp::EntityId sourceEid = TEST_GROUND_EID;
    Cfdp::FileSize fileSize = 1000;
    Cfdp::TransactionSeq seq = 1;

    // Move SOURCE: the transaction's initial dst_filename. Make it a DIRECTORY so that
    // rename(source_dir -> dest) succeeds and turns the DEST into a directory.
    const char* moveSourceDir = "test/ut/output/reopen_src_dir";
    // Move DEST: comes from the Metadata PDU. After the rename it is a directory; opening a
    // directory for writing then fails with EISDIR.
    const char* moveDest = "test/ut/output/reopen_dst_dir";

    // Make sure neither path pre-exists (clean slate), then create the source directory on disk.
    Os::FileSystem::removeDirectory(moveDest);
    Os::FileSystem::removeDirectory(moveSourceDir);
    Os::FileSystem::Status mkStatus = Os::FileSystem::createDirectory(moveSourceDir);
    ASSERT_EQ(Os::FileSystem::OP_OK, mkStatus);

    // Set up Class 2 receiver transaction; its dst_filename is the move SOURCE (the directory)
    Transaction* txn = setupTestTransaction(TxnState::TXN_STATE_R2,  // Class 2 receiver
                                            channelId,
                                            "test_src.dat",  // srcFilename (unused for rename)
                                            moveSourceDir,   // dstFilename -> becomes the move SOURCE (fname)
                                            fileSize, seq, sourceEid);

    txn->m_engine = this->component.m_engine;
    txn->m_history->src_eid = sourceEid;

    // Force the "metadata arriving late" rename path:
    txn->m_flags.rx.md_recv = false;   // metadata not yet received -> enter rename branch
    txn->m_flags.rx.eof_recv = false;  // skip EOF/MD size-mismatch check -> keep success==true

    // Build the Metadata PDU whose destination filename is the rename target
    Cfdp::MetadataPdu metadataPdu;
    metadataPdu.initialize(Cfdp::PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                           sourceEid,                           // sourceEid
                           seq,                                 // transactionSeq
                           this->component.getLocalEidParam(),  // destEid
                           fileSize,                            // fileSize
                           "test_src.dat",                      // source filename
                           moveDest,                            // dest filename -> move DEST (becomes a directory)
                           Cfdp::ChecksumType::CHECKSUM_TYPE_MODULAR,
                           1);  // closureRequested for Class 2

    // Serialize PDU to buffer (direct r2RecvMd call: no packet descriptor prefix)
    U8 tempBuffer[300];
    Fw::SerialBuffer sb(tempBuffer, sizeof(tempBuffer));
    metadataPdu.serializeTo(sb);
    Fw::Buffer pduBuffer(tempBuffer, sb.getSize());

    this->clearEvents();

    // Directly call r2RecvMd -> parses MD (MetadataReceived), rename of the directory succeeds,
    // then reopen of the (now-directory) dest for writing fails -> RxFileReopenFailed.
    txn->r2RecvMd(pduBuffer);

    // Verify events: MetadataReceived (from recvMd) + RxFileReopenFailed (reopen failure)
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_MetadataReceived_SIZE(1);
    ASSERT_EVENTS_RxFileReopenFailed_SIZE(1);

    // Cleanup: rename succeeded, so the source no longer exists; the dest is now the directory.
    Os::FileSystem::removeDirectory(moveDest);
    Os::FileSystem::removeDirectory(moveSourceDir);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
