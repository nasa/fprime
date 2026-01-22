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
#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>

namespace Svc {

namespace Ccsds {

class CfdpManagerTester final : public CfdpManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

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
    // PDU Test Helper Functions
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
    CF_Transaction_t* setupTestTransaction(
        CF_TxnState_t state,
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
    void verifyMetadataPdu(
        const Fw::Buffer& pduBuffer,
        U32 expectedSourceEid,
        U32 expectedDestEid,
        U32 expectedTransactionSeq,
        CfdpFileSize expectedFileSize,
        const char* expectedSourceFilename,
        const char* expectedDestFilename
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
        const char* filename
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
        CfdpFileSize expectedFileSize,
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
        CfdpFileSize expectedScopeStart,
        CfdpFileSize expectedScopeEnd,
        U8 expectedNumSegments = 0,
        const Cfdp::Pdu::SegmentRequest* expectedSegments = nullptr
    );

  private:
    // ----------------------------------------------------------------------
    //  Test Harness: output port overrides
    // ----------------------------------------------------------------------

    //! Handler for from_bufferAllocate - allocates test buffers
    Fw::Buffer from_bufferAllocate_handler(
        FwIndexType portNum,
        FwSizeType size
    ) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    CfdpManager component;

    //! Reusable buffer for allocation handler
    U8 m_internalDataBuffer[CF_MAX_PDU_SIZE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
