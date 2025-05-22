// ======================================================================
// \title  SpacePacketDeframerTester.hpp
// \author chammard
// \brief  hpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_SpacePacketDeframerTester_HPP
#define Svc_CCSDS_SpacePacketDeframerTester_HPP

#include "Svc/CCSDS/SpacePacketDeframer/SpacePacketDeframer.hpp"
#include "Svc/CCSDS/SpacePacketDeframer/SpacePacketDeframerGTestBase.hpp"

namespace Svc {

namespace CCSDS {

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

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SpacePacketDeframer component;
};

}  // namespace CCSDS

}  // namespace Svc

#endif
