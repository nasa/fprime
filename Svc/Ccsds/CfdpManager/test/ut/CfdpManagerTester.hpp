// ======================================================================
// \title  CfdpManagerTester.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_CfdpManagerTester_HPP
#define Svc_Ccsds_CfdpManagerTester_HPP

#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManagerGTestBase.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>
#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

class CfdpManagerTester final : public CfdpManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 100;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object CfdpManagerTester
    CfdpManagerTester();

    //! Destroy object CfdpManagerTester
    ~CfdpManagerTester();

  public:
    // ----------------------------------------------------------------------
    // White Box PDU Tests
    // ----------------------------------------------------------------------

    //! Test generating a Metadata PDU
    void testMetaDataPdu();

    //! Test generating a File Data PDU
    void testFileDataPdu();

    //! Test generating an EOF PDU
    void testEofPdu();

    //! Test generating a FIN PDU
    void testFinPdu();

    //! Test generating an ACK PDU
    void testAckPdu();

    //! Test generating a NAK PDU
    void testNakPdu();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    // ----------------------------------------------------------------------
    // PDU Downlink Test Helper Functions
    // ----------------------------------------------------------------------

    //! Helper to create a minimal transaction for testing
    //! @param state Transaction state (S1, S2, R1, R2, etc.)
    //! @param channelId CFDP channel number (0-based)
    //! @param srcFilename Source filename for the transfer
    //! @param dstFilename Destination filename for the transfer
    //! @param fileSize File size in octets
    //! @param sequenceId Transaction sequence number
    //! @param peerId Peer entity ID
    //! @return Pointer to configured transaction (owned by component)
    CfdpTransaction* setupTestTransaction(
        TxnState state,
        U8 channelId,
        const char* srcFilename,
        const char* dstFilename,
        U32 fileSize,
        U32 sequenceId,
        U32 peerId
    );

    //! Helper to get PDU buffer from dataOut port history
    //! @param index History index (0 for most recent)
    //! @return Reference to the buffer
    const Fw::Buffer& getSentPduBuffer(FwIndexType index);

    //! Helper to verify Metadata PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedFileSize Expected file size
    //! @param expectedSourceFilename Expected source filename
    //! @param expectedDestFilename Expected destination filename
    //! @param expectedClass Expected CFDP class (CLASS_1 or CLASS_2)
    void verifyMetadataPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        FileSize expectedFileSize,
        const char* expectedSourceFilename,
        const char* expectedDestFilename,
        Svc::Ccsds::Cfdp::Class::T expectedClass
    );

    //! Helper to verify File Data PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedOffset Expected file offset
    //! @param expectedDataSize Expected data size
    //! @param filename Source file to read expected data from
    void verifyFileDataPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        U32 expectedOffset,
        U16 expectedDataSize,
        const char* filename,
        Svc::Ccsds::Cfdp::Class::T expectedClass
    );

    //! Helper to verify EOF PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedConditionCode Expected condition code
    //! @param expectedFileSize Expected file size
    //! @param sourceFilename Source file path to compute CRC for validation
    void verifyEofPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        Cfdp::ConditionCode expectedConditionCode,
        FileSize expectedFileSize,
        const char* sourceFilename
    );

    //! Helper to verify FIN PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedConditionCode Expected condition code
    //! @param expectedDeliveryCode Expected delivery code
    //! @param expectedFileStatus Expected file status
    void verifyFinPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        Cfdp::ConditionCode expectedConditionCode,
        Cfdp::FinDeliveryCode expectedDeliveryCode,
        Cfdp::FinFileStatus expectedFileStatus
    );

    //! Helper to verify ACK PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedDirectiveCode Expected directive code being acknowledged
    //! @param expectedDirectiveSubtypeCode Expected directive subtype code
    //! @param expectedConditionCode Expected condition code
    //! @param expectedTransactionStatus Expected transaction status
    void verifyAckPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        Cfdp::FileDirective expectedDirectiveCode,
        U8 expectedDirectiveSubtypeCode,
        Cfdp::ConditionCode expectedConditionCode,
        Cfdp::AckTxnStatus expectedTransactionStatus
    );

    //! Helper to verify NAK PDU (deserialize + validate)
    //! @param pduBuffer Buffer containing complete PDU bytes (header + body)
    //! @param expectedSourceEid Expected source entity ID
    //! @param expectedDestEid Expected destination entity ID
    //! @param expectedTransactionSeq Expected transaction sequence number
    //! @param expectedScopeStart Expected scope start offset
    //! @param expectedScopeEnd Expected scope end offset
    //! @param expectedNumSegments Expected number of segment requests (0 = skip segment validation)
    //! @param expectedSegments Optional array of expected segment requests (only used if expectedNumSegments > 0)
    void verifyNakPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        FileSize expectedScopeStart,
        FileSize expectedScopeEnd,
        U8 expectedNumSegments,
        const Cfdp::SegmentRequest* expectedSegments
    );

    //! Helper to find transaction by sequence number
    //! @param chanNum Channel number to search
    //! @param seqNum Transaction sequence number
    //! @return Pointer to transaction or nullptr if not found
    CfdpTransaction* findTransaction(U8 chanNum, TransactionSeq seqNum);

    // ----------------------------------------------------------------------
    // PDU Uplink Helper Functions
    // ----------------------------------------------------------------------

    //! Helper to send a Metadata PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground)
    //! @param destEid Destination entity ID (FSW)
    //! @param transactionSeq Transaction sequence number
    //! @param fileSize File size in octets
    //! @param sourceFilename Source filename
    //! @param destFilename Destination filename
    //! @param class Transmission mode (Class 1 or Class 2)
    //! @param closureRequested Closure requested flag (typically 0 for Class 1, 1 for Class 2)
    void sendMetadataPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        FileSize fileSize,
        const char* sourceFilename,
        const char* destFilename,
        Cfdp::Class::T txmMode,
        U8 closureRequested
    );

    //! Helper to send a File Data PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground)
    //! @param destEid Destination entity ID (FSW)
    //! @param transactionSeq Transaction sequence number
    //! @param offset File offset
    //! @param dataSize Data size in octets
    //! @param data Pointer to file data
    //! @param class Transmission mode (Class 1 or Class 2)
    void sendFileDataPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        FileSize offset,
        U16 dataSize,
        const U8* data,
        Cfdp::Class::T txmMode
    );

    //! Helper to send an EOF PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground)
    //! @param destEid Destination entity ID (FSW)
    //! @param transactionSeq Transaction sequence number
    //! @param conditionCode Condition code
    //! @param checksum File checksum
    //! @param fileSize File size in octets
    //! @param class Transmission mode (Class 1 or Class 2)
    void sendEofPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        Cfdp::ConditionCode conditionCode,
        U32 checksum,
        FileSize fileSize,
        Cfdp::Class::T txmMode
    );

    //! Helper to send a FIN PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground as receiver)
    //! @param destEid Destination entity ID (FSW as sender)
    //! @param transactionSeq Transaction sequence number
    //! @param conditionCode Condition code
    //! @param deliveryCode Delivery code
    //! @param fileStatus File status
    void sendFinPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        Cfdp::ConditionCode conditionCode,
        Cfdp::FinDeliveryCode deliveryCode,
        Cfdp::FinFileStatus fileStatus
    );

    //! Helper to send an ACK PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground as receiver)
    //! @param destEid Destination entity ID (FSW as sender)
    //! @param transactionSeq Transaction sequence number
    //! @param directiveCode Directive being acknowledged
    //! @param directiveSubtypeCode Directive subtype code
    //! @param conditionCode Condition code
    //! @param transactionStatus Transaction status
    void sendAckPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        Cfdp::FileDirective directiveCode,
        U8 directiveSubtypeCode,
        Cfdp::ConditionCode conditionCode,
        Cfdp::AckTxnStatus transactionStatus
    );

    //! Helper to send a NAK PDU to CfdpManager via dataIn
    //! @param channelId CFDP channel number
    //! @param sourceEid Source entity ID (ground as receiver)
    //! @param destEid Destination entity ID (FSW as sender)
    //! @param transactionSeq Transaction sequence number
    //! @param scopeStart Scope start offset
    //! @param scopeEnd Scope end offset
    //! @param numSegments Number of segment requests (0 to CF_NAK_MAX_SEGMENTS)
    //! @param segments Array of segment requests (can be nullptr if numSegments is 0)
    void sendNakPdu(
        U8 channelId,
        EntityId sourceEid,
        EntityId destEid,
        TransactionSeq transactionSeq,
        FileSize scopeStart,
        FileSize scopeEnd,
        U8 numSegments,
        const Cfdp::SegmentRequest* segments
    );

  public:
    // ----------------------------------------------------------------------
    // Transaction Tests
    // ----------------------------------------------------------------------

    //! Test nominal Class 1 TX file transfer
    void testClass1TxNominal();

    //! Test nominal Class 2 TX file transfer
    void testClass2TxNominal();

    //! Test Class 2 TX file transfer with NAK handling
    void testClass2TxNack();

    //! Test nominal Class 1 RX file transfer
    void testClass1RxNominal();

    //! Test nominal Class 2 RX file transfer
    void testClass2RxNominal();

    //! Test Class 2 RX file transfer with NAK retransmission
    void testClass2RxNack();

  private:
    // ----------------------------------------------------------------------
    //  Test Harness: output port overrides
    // ----------------------------------------------------------------------

    //! Handler for from_bufferAllocate - allocates test buffers
    Fw::Buffer from_bufferAllocate_handler(
        FwIndexType portNum,
        FwSizeType size
    ) override;

    //! Handler for from_dataOut - copies PDU data to avoid buffer reuse issues
    void from_dataOut_handler(
        FwIndexType portNum,
        Fw::Buffer& fwBuffer
    ) override;

  private:
    // ----------------------------------------------------------------------
    // Test helper functions
    // ----------------------------------------------------------------------

    //! Test configuration constants
    static constexpr U8 TEST_CHANNEL_ID_0 = 0;
    static constexpr U8 TEST_CHANNEL_ID_1 = 1;
    static constexpr EntityId TEST_GROUND_EID = 10;
    static constexpr U8 TEST_PRIORITY = 0;

    //! Helper struct for transaction setup results
    struct TransactionSetup {
        U32 expectedSeqNum;
        CfdpTransaction* txn;
    };

    //! Create test file and verify size matches expected
    void createAndVerifyTestFile(
        const char* filePath,
        FwSizeType expectedFileSize,
        FwSizeType& actualFileSize
    );

    //! Setup TX transaction and verify initial state
    void setupTxTransaction(
        const char* srcFile,
        const char* dstFile,
        U8 channelId,
        EntityId destEid,
        Cfdp::Class cfdpClass,
        U8 priority,
        TxnState expectedState,
        TransactionSetup& setup
    );

    //! Setup RX transaction via Metadata PDU and verify initial state
    void setupRxTransaction(
        const char* srcFile,
        const char* dstFile,
        U8 channelId,
        EntityId sourceEid,
        Cfdp::Class::T cfdpClass,
        U32 fileSize,
        U32 transactionSeq,
        TxnState expectedState,
        TransactionSetup& setup
    );

    //! Wait for transaction to be recycled by inactivity timer
    void waitForTransactionRecycle(U8 channelId, U32 expectedSeqNum);

    //! Complete Class 2 transaction handshake (EOF-ACK, FIN, FIN-ACK)
    void completeClass2Handshake(
        U8 channelId,
        EntityId destEid,
        U32 expectedSeqNum,
        CfdpTransaction* txn
    );

    //! Verify FIN-ACK PDU at given index
    void verifyFinAckPdu(
        FwIndexType pduIndex,
        EntityId sourceEid,
        EntityId destEid,
        U32 expectedSeqNum
    );

    //! Verify Metadata PDU at specific index in port history
    void verifyMetadataPduAtIndex(
        FwIndexType pduIndex,
        const TransactionSetup& setup,
        FwSizeType fileSize,
        const char* srcFile,
        const char* dstFile,
        Cfdp::Class::T cfdpClass
    );

    //! Verify multiple FileData PDUs in sequence
    void verifyMultipleFileDataPdus(
        FwIndexType startIndex,
        U8 numPdus,
        const TransactionSetup& setup,
        U16 dataPerPdu,
        const char* srcFile,
        Cfdp::Class::T cfdpClass
    );

    //! Clean up test file (remove and verify)
    void cleanupTestFile(const char* filePath);

    //! Verify received file matches expected data
    void verifyReceivedFile(
        const char* filePath,
        const U8* expectedData,
        FwSizeType expectedSize
    );

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    CfdpManager component;

    //! Reusable buffer for allocation handler
    U8 m_internalDataBuffer[CFDP_MAX_PDU_SIZE];

    //! Storage for PDU copies (to avoid buffer reuse issues)
    static constexpr FwSizeType MAX_PDU_COPIES = 100;
    U8 m_pduCopyStorage[MAX_PDU_COPIES][CFDP_MAX_PDU_SIZE];
    FwSizeType m_pduCopyCount;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif
