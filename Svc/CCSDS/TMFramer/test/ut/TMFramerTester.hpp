// ======================================================================
// \title  TMFramerTester.hpp
// \author thomas-bc
// \brief  hpp file for TMFramer component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_TMFramerTester_HPP
#define Svc_CCSDS_TMFramerTester_HPP

#include "Svc/CCSDS/TMFramer/TMFramer.hpp"
#include "Svc/CCSDS/TMFramer/TMFramerGTestBase.hpp"

namespace Svc {

namespace CCSDS {

class TMFramerTester final : public TMFramerGTestBase {
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

    //! Construct object TMFramerTester
    TMFramerTester();

    //! Destroy object TMFramerTester
    ~TMFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testNominalFraming();
    void testSeqCountWrapAround();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    U16 getFrameScId(U8* frameData);    //!< Get the Spacecraft ID from the frame - no boundary check
    U8 getFrameVcId(U8* frameData);     //!< Get the Virtual Channel ID from the frame - no boundary check
    U8 getFrameMcCount(U8* frameData);  //!< Get the Master Frame Count from the frame - no boundary check
    U8 getFrameVcCount(U8* frameData);  //!< Get the Virtual Frame Count from the frame - no boundary check

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    TMFramer component;
};

}  // namespace CCSDS

}  // namespace Svc

#endif
