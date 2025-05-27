// ======================================================================
// \title  TCDeframerTester.hpp
// \author thomas-bc
// \brief  hpp file for TCDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_TCDeframerTester_HPP
#define Svc_CCSDS_TCDeframerTester_HPP

#include "Svc/CCSDS/TCDeframer/TCDeframer.hpp"
#include "Svc/CCSDS/TCDeframer/TCDeframerGTestBase.hpp"

namespace Svc {

namespace CCSDS {

class TCDeframerTester final : public TCDeframerGTestBase {
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

    //! Construct object TCDeframerTester
    TCDeframerTester();

    //! Destroy object TCDeframerTester
    ~TCDeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testDataReturn();
    void testNominalDeframing();
    void testInvalidScId();
    void testInvalidVcId();
    void testInvalidLengthToken();
    // void testInvalidSequenceNumber();
    void testInvalidCrc();


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

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    TCDeframer component;

    U8 m_frameData[300]; // data buffer used to produce test frames
};

}  // namespace CCSDS

}  // namespace Svc

#endif
