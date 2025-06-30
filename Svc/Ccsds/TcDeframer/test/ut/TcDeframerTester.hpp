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
    static const FwSizeType MAX_HISTORY_SIZE = 10;

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
    TcDeframer component;

    U8 m_frameData[300];  // data buffer used to produce test frames
};

}  // namespace Ccsds

}  // namespace Svc

#endif
