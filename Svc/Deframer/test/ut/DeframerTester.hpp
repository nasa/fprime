// ======================================================================
// \title  DeframerTester.hpp
// \author thomas-bc
// \brief  hpp file for Deframer component test harness implementation class
// ======================================================================

#ifndef Svc_DeframerTester_HPP
#define Svc_DeframerTester_HPP

#include "Svc/Deframer/Deframer.hpp"
#include "Svc/Deframer/DeframerGTestBase.hpp"

namespace Svc {

class DeframerTester : public DeframerGTestBase {
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

    //! Construct object DeframerTester
    DeframerTester();

    //! Destroy object DeframerTester
    ~DeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test receiving a nominal frame
    void testNominalFrame();

    //! Test receiving a truncated frame
    void testTruncatedFrame();

    //! Test receiving a zero size frame
    void testZeroSizeFrame();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    void mockReceiveData(U8* data, FwSizeType size);


  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    Deframer component;
};

}  // namespace Svc

#endif
