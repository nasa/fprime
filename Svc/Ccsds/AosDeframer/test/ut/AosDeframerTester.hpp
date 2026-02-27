// ======================================================================
// \title  AosDeframerTester.hpp
// \author Auto-generated
// \brief  hpp file for AosDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AosDeframerTester_HPP
#define Svc_Ccsds_AosDeframerTester_HPP

#include "Svc/Ccsds/AosDeframer/AosDeframer.hpp"
#include "Svc/Ccsds/AosDeframer/AosDeframerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AosDeframerTester final : public AosDeframerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Test frame sizes
    static const U32 TEST_FRAME_SIZE = 256;
    static const U32 TEST_FRAME_SIZE_LARGE = 1024;
    static const FwSizeType ALLOC_BUF_SIZE = 65536;

    // Data zone size within TEST_FRAME_SIZE frames (header + M_PDU header + FECF trailer)
    static const FwSizeType TEST_DATA_ZONE_SIZE =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object AosDeframerTester
    AosDeframerTester();

    //! Destroy object AosDeframerTester
    ~AosDeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests - Basic Validation
    // ----------------------------------------------------------------------

    //! Test nominal deframing with single SPP packet
    void testNominalDeframing();

    //! Test data return passthrough
    void testDataReturn();

    //! Test invalid spacecraft ID handling
    void testInvalidScId();

    //! Test invalid virtual channel ID handling
    void testInvalidVcId();

    //! Test invalid frame length handling
    void testInvalidFrameLength();

    //! Test invalid FECF (CRC) handling
    void testInvalidFecf();

    //! Test invalid transfer frame version number
    void testInvalidTfvn();

    //! Test VC frame count gap detection emits event + errorNotify
    void testVcFrameCountGap();

    // Accept-all-VCID is not supported: each VC struct maps to exactly one VCID,
    // enabling per-packet spanning state tracking.

    // ----------------------------------------------------------------------
    // Tests - M_PDU Processing
    // ----------------------------------------------------------------------

    //! Test First Header Pointer at non-zero offset
    void testFhpAtOffset();

    //! Test FHP_NO_PACKET_START (0x7FE) - continuation only
    void testFhpNoPacketStart();

    //! Test FHP_IDLE_DATA_ONLY (0x7FF) - idle frame
    void testFhpIdleDataOnly();

    //! Test multiple packets in single frame
    void testMultiplePacketsInFrame();

    // ----------------------------------------------------------------------
    // Tests - Spanning Packets
    // ----------------------------------------------------------------------

    //! Test packet spanning across two frames
    void testSpanningPacketTwoFrames();

    //! Test packet spanning across four frames (explicit 3+ frame coverage)
    void testSpanningPacketFourFrames();

    //! Test spanning packet with continuation frame
    void testSpanningPacketContinuation();

    //! Test spanning packet allocation failure emits an event and drops the packet
    void testSpanningPacketAllocFailureEvent();

    //! Test spanning packet dropped when a VC frame count gap is detected mid-reassembly
    void testSpanningPacketAbandonedOnVcGap();

    //! Test spanning packet dropped when an idle frame arrives mid-reassembly
    void testSpanningPacketAbandonedOnIdleFrame();

    // ----------------------------------------------------------------------
    // Tests - SPP Extraction
    // ----------------------------------------------------------------------

    //! Test SPP idle packet filtering
    void testSppIdlePacketFiltering();

    // ----------------------------------------------------------------------
    // Tests - EPP Extraction
    // ----------------------------------------------------------------------

    //! Test Encapsulation Packet Protocol extraction
    void testEppExtraction();

    //! Test EPP idle packet handling
    void testEppIdlePacket();

    //! Test EPP fill packet handling
    void testEppFillPacket();

    //! Test invalid packet version
    void testInvalidPvnVersion();

    // ----------------------------------------------------------------------
    // Tests - Configuration
    // ----------------------------------------------------------------------

    //! Test FECF disabled mode
    void testFecfDisabled();

    //! Test PVN mask filtering (SPP only)
    void testPvnMaskSppOnly();

    //! Test PVN mask filtering (EPP only)
    void testPvnMaskEppOnly();

    // ----------------------------------------------------------------------
    // Tests - Telemetry
    // ----------------------------------------------------------------------

    //! Test frame count telemetry
    void testFrameCountTelemetry();

  private:
    // ----------------------------------------------------------------------
    // From-port handlers (allocator support for spanning packets)
    // ----------------------------------------------------------------------

    Fw::Buffer from_allocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Configure the component with default test settings
    void configureDefault();

    //! Assemble an AOS frame buffer with the given parameters
    //! \param payload Pointer to M_PDU payload data
    //! \param payloadLength Length of payload data
    //! \param fhp First Header Pointer value
    //! \param scid Spacecraft ID
    //! \param vcid Virtual Channel ID
    //! \param vcCount Virtual Channel Frame Count
    //! \param tfvn Transfer Frame Version Number
    //! \param includeFecf Whether to include FECF
    //! \return Assembled frame buffer
    Fw::Buffer assembleFrameBuffer(U8* payload,
                                   FwSizeType payloadLength,
                                   U16 fhp = 0,
                                   U16 scid = ComCfg::SpacecraftId,
                                   U8 vcid = 0,
                                   U32 vcCount = 0,
                                   U8 tfvn = 1,
                                   bool includeFecf = true);

    //! Create an SPP packet in the buffer
    //! \param buffer Destination buffer
    //! \param apid Application Process ID
    //! \param dataLength Packet data length (not including header)
    //! \param seqCount Sequence count
    //! \return Total packet size
    FwSizeType createSppPacket(U8* buffer, U16 apid, U16 dataLength, U16 seqCount = 0);

    //! Create an EPP packet in the buffer
    //! \param buffer Destination buffer
    //! \param protocolId Protocol ID (0 = Idle per EppProtocolId::Idle)
    //! \param lengthOfLength Number of length field bytes (0=1 byte idle, 1, 2, or 4)
    //! \param dataLength Packet data length
    //! \return Total packet size
    FwSizeType createEppPacket(U8* buffer, U8 protocolId, U8 lengthOfLength, FwSizeType dataLength);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AosDeframer component;

    //! Data buffer used to produce test frames
    U8 m_frameData[ComCfg::AosMaxFrameFixedSize];

    //! Static backing storage returned by the allocate port in unit tests
    U8 m_allocBuf[ALLOC_BUF_SIZE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
