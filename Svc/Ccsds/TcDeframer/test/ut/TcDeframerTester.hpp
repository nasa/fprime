// ======================================================================
// \title  TcDeframerTester.hpp
// \author thomas-bc
// \brief  hpp file for TcDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_TcDeframerTester_HPP
#define Svc_Ccsds_TcDeframerTester_HPP

#include "Svc/Ccsds/TcDeframer/TcDeframer.hpp"
#include "Svc/Ccsds/TcDeframer/TcDeframerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class TcDeframerTester final : public TcDeframerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 100;

    // Size of each buffer handed out by the test allocate port
    static const FwSizeType ALLOC_BUF_SIZE = 512;

    // Number of buffers the test allocate port can hand out before wrapping around: enough to fill the in-flight
    // table, plus one for the packet dropped at the limit and one for the packet delivered after a slot is freed
    static const FwSizeType ALLOC_POOL_COUNT = TcDeframerCfg::MaxSpanningPacketsInFlight + 2;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object TcDeframerTester
    TcDeframerTester();

    //! Destroy object TcDeframerTester
    ~TcDeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testDataReturn();
    void testNominalDeframing();
    void testInvalidScId();
    void testInvalidVcId();
    void testInvalidLengthToken();
    void testInvalidCrc();

    void testSegmentedUnsegmentedPassthrough();
    void testSegmentedTwoSegments();
    void testSegmentedManySegments();
    void testSegmentedUnexpectedSegment();
    void testSegmentedFirstRestartsPacket();
    void testSegmentedUnsegmentedAbandonsPacket();
    void testSegmentedInvalidMapId();
    void testSegmentedAllocFailure();
    void testSegmentedOverflow();
    void testSegmentedExactFill();
    void testSegmentedShortAllocation();
    void testSegmentedInFlightLimit();
    void testSegmentedEmptySegment();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Hand out a buffer from the test pool, or an invalid buffer when a failure is requested
    Fw::Buffer from_allocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Sets the component state to specific values, helpful for testing
    void setComponentState(U16 scid = 0, U8 vcid = 0, U8 seqNumber = 0, bool acceptAllVcid = true);

    Fw::Buffer assembleFrameBuffer(U8* data, U8 dataLength, U16 scid = 0, U8 vcid = 0, U8 seqNumber = 0);

    //! Enables segment header processing on the component under test
    void enableSegmentation(U8 mapId = 0, FwSizeType maxSpanningPacketSize = ALLOC_BUF_SIZE);

    //! Assembles a valid TC frame whose data field is [segment header | data]
    Fw::Buffer assembleSegmentFrameBuffer(U8 sequenceFlags, U8 mapId, const U8* data, U8 dataLength, U8 vcid = 0);

    //! Fills data with a deterministic pattern derived from seed
    static void fillPattern(U8* data, FwSizeType length, U8 seed);

    //! Sends a segment and checks it was consumed: frame returned upstream, nothing emitted downstream
    void sendConsumedSegment(U8 sequenceFlags, const U8* data, U8 dataLength);

    //! Checks that the single dataOut emission holds exactly the expected bytes
    void assertDataOutEquals(const U8* expected, FwSizeType expectedLength);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    TcDeframer component;

    U8 m_frameData[300] = {};  // data buffer used to produce test frames

    U8 m_segmentData[256] = {};  // scratch buffer holding [segment header | data] while building a frame

    U8 m_allocPool[ALLOC_POOL_COUNT][ALLOC_BUF_SIZE] = {};  // backing storage returned by the allocate port

    FwSizeType m_nextAlloc = 0;  // index of the next pool buffer to hand out

    U8 m_mapId = 0;  // MAP ID the component was configured with by enableSegmentation

    bool m_failNextAlloc = false;  // when true, the next allocate call returns an invalid buffer

    FwSizeType m_shortNextAlloc = 0;  // when non-zero, the next allocate call returns a buffer of this smaller size
};

}  // namespace Ccsds

}  // namespace Svc

#endif
