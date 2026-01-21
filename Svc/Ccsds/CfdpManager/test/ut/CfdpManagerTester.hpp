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
#include <Svc/Ccsds/Types/CfdpPduHeaderSerializableAc.hpp>
#include <Svc/Ccsds/Types/CfdpFileDirectiveHeaderSerializableAc.hpp>
#include <Svc/Ccsds/Types/CfdpMetadataPduSerializableAc.hpp>

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

    //! Helper to deserialize and validate PDU header
    //! @param pduBuffer Buffer containing PDU bytes
    //! @param header Output: deserialized header
    //! @return True if deserialization successful
    bool deserializePduHeader(
        const Fw::Buffer& pduBuffer,
        Svc::Ccsds::CfdpPduHeader& header
    );

    //! Helper to validate PDU header flags
    //! @param flags Bit-packed flags byte from header
    //! @param expectedVersion Expected CFDP version (should be 1)
    //! @param expectedPduType Expected PDU type (0=directive, 1=file data)
    //! @param expectedDirection Expected direction (0=toward receiver, 1=toward sender)
    //! @param expectedTxMode Expected transmission mode (0=ack, 1=unack)
    //! @return True if all flags match expected values
    bool validateHeaderFlags(
        U8 flags,
        U8 expectedVersion,
        U8 expectedPduType,
        U8 expectedDirection,
        U8 expectedTxMode
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
