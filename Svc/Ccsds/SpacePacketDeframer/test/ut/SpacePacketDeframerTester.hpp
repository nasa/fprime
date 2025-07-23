// ======================================================================
// \title  SpacePacketDeframerTester.hpp
// \author thomas-bc
// \brief  hpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_SpacePacketDeframerTester_HPP
#define Svc_Ccsds_SpacePacketDeframerTester_HPP

#include "Svc/Ccsds/SpacePacketDeframer/SpacePacketDeframer.hpp"
#include "Svc/Ccsds/SpacePacketDeframer/SpacePacketDeframerGTestBase.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

class SpacePacketDeframerTester final : public SpacePacketDeframerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object SpacePacketDeframerTester
    SpacePacketDeframerTester();

    //! Destroy object SpacePacketDeframerTester
    ~SpacePacketDeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testDataReturnPassthrough();
    void testNominalDeframing();
    void testDeframingIncorrectLength();
    // void testDeframingIncorrectSeqCount();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Assemble a packet with the given parameters
    Fw::Buffer assemblePacket(U16 apid, U16 seqCount, U16 lengthToken, U8* packetData, U16 packetDataLen);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SpacePacketDeframer component;

    //! Test buffer
    static const FwSizeType MAX_TEST_PACKET_DATA_SIZE = 200;  // this value needs to fit in a U8 for testing
    U8 m_packetBuffer[SpacePacketHeader::SERIALIZED_SIZE + MAX_TEST_PACKET_DATA_SIZE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
