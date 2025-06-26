// ======================================================================
// \title  FprimeFramerTester.hpp
// \author thomas-bc
// \brief  hpp file for FprimeFramer component test harness implementation class
// ======================================================================

#ifndef Svc_FprimeFramerTester_HPP
#define Svc_FprimeFramerTester_HPP

#include "Svc/FprimeFramer/FprimeFramer.hpp"
#include "Svc/FprimeFramer/FprimeFramerGTestBase.hpp"

namespace Svc {

class FprimeFramerTester final : public FprimeFramerGTestBase {
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

    //! Construct object FprimeFramerTester
    FprimeFramerTester();

    //! Destroy object FprimeFramerTester
    ~FprimeFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test pass through of comStatusIn to comStatusOut
    void testComStatusPassThrough();

    //! Test deallocation of data
    void testFrameDeallocation();

    //! Test framing of data
    void testNominalFraming();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    // ----------------------------------------------------------------------
    // Test Harness: Handler implementations for output ports
    // ----------------------------------------------------------------------

    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FprimeFramer component;

    U8 m_buffer_slot[2048];
    Fw::Buffer m_buffer;  // buffer to be returned by mocked allocate call
};

}  // namespace Svc

#endif
