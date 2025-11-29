// ======================================================================
// \title  AosDeframerTester.hpp
// \author thomas-bc
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
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

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
    AosDeframer component;

    U8 m_frameData[300];  // data buffer used to produce test frames
};

}  // namespace Ccsds

}  // namespace Svc

#endif
