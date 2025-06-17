// ======================================================================
// \title  TmFramerTester.hpp
// \author thomas-bc
// \brief  hpp file for TmFramer component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_TmFramerTester_HPP
#define Svc_CCSDS_TmFramerTester_HPP

#include "Svc/CCSDS/TmFramer/TmFramer.hpp"
#include "Svc/CCSDS/TmFramer/TmFramerGTestBase.hpp"

namespace Svc {

namespace CCSDS {

class TmFramerTester final : public TmFramerGTestBase {
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

    //! Construct object TmFramerTester
    TmFramerTester();

    //! Destroy object TmFramerTester
    ~TmFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testNominalFraming();
    void testSeqCountWrapAround();
    void testInputBufferTooLarge();
    void testDataReturn();
    void testBufferOwnershipState();

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
    TmFramer component;
};

}  // namespace CCSDS

}  // namespace Svc

#endif
