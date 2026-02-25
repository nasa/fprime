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

    //! Test First Header Pointer at offset 0
    void testFhpAtZero();

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

    //! Test packet spanning across multiple frames
    void testSpanningPacketMultipleFrames();

    //! Test packet spanning across four frames (explicit 3+ frame coverage)
    void testSpanningPacketFourFrames();

    //! Test spanning packet with continuation frame
    void testSpanningPacketContinuation();

    // ----------------------------------------------------------------------
    // Tests - SPP Extraction
    // ----------------------------------------------------------------------

    //! Test Space Packet Protocol extraction
    void testSppExtraction();

    //! Test SPP idle packet filtering
    void testSppIdlePacketFiltering();

    //! Test SPP with sequence count extraction
    void testSppSequenceCount();

    // ----------------------------------------------------------------------
    // Tests - EPP Extraction
    // ----------------------------------------------------------------------

    //! Test Encapsulation Packet Protocol extraction
    void testEppExtraction();

    //! Test EPP idle packet handling
    void testEppIdlePacket();

    //! Test EPP fill packet handling
    void testEppFillPacket();

    //! Test invalid EPP packet version
    void testInvalidEppVersion();

    // ----------------------------------------------------------------------
    // Tests - Private Helper Edge Cases (friend access)
    // ----------------------------------------------------------------------

    //! Directly exercise parseAndValidateHeader deserializer failure path
    void testHeaderDeserializeFailureHelperPath();

    //! Directly exercise short/incomplete packet guard paths in SPP/EPP extractors
    void testExtractorGuardPaths();

    //! Directly exercise extended EPP protocol branch (protocol ID 0x8-0xF)
    void testExtendedEppProtocolBranch();

    //! Directly exercise EPP branch of appendToSpanningPacket completion context tagging
    void testAppendToSpanningPacketEppCompletion();

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

    //! Test packet count telemetry
    void testPacketCountTelemetry();

    //! Test CRC error count telemetry
    void testCrcErrorCountTelemetry();

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
    //! \param protocolId Protocol ID
    //! \param dataLength Packet data length
    //! \return Total packet size
    FwSizeType createEppPacket(U8* buffer, U8 protocolId, U16 dataLength);

    //! Create an EPP idle packet in the buffer
    //! \param buffer Destination buffer
    //! \param lengthOfLength Length of the length field (0 for fill packet)
    //! \param packetLength Packet length (if lengthOfLength > 0)
    //! \return Total packet size
    FwSizeType createEppIdlePacket(U8* buffer, U8 lengthOfLength, FwSizeType packetLength);

    //! Get the frame's TFVN from raw data
    U8 getFrameTfvn(U8* frameData);

    //! Get the frame's Spacecraft ID from raw data
    U16 getFrameScId(U8* frameData);

    //! Get the frame's Virtual Channel ID from raw data
    U8 getFrameVcId(U8* frameData);

    //! Get the frame's VC Frame Count from raw data
    U32 getFrameVcCount(U8* frameData);

    //! Get the frame's First Header Pointer from raw data
    U16 getFrameFhp(U8* frameData);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AosDeframer component;

    //! Data buffer used to produce test frames
    U8 m_frameData[ComCfg::AosMaxFrameFixedSize];

    //! Secondary frame buffer for multi-frame tests
    U8 m_frameData2[ComCfg::AosMaxFrameFixedSize];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
