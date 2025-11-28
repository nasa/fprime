// ======================================================================
// \title  ComRetryTester.hpp
// \author valdaarhun
// \brief  hpp file for ComRetry test harness implementation class
// ======================================================================

#ifndef Svc_ComRetryTester_HPP
#define Svc_ComRetryTester_HPP

#include "ComRetryGTestBase.hpp"
#include "Svc/ComRetry/ComRetry.hpp"

#define BUFFER_LENGTH 3u
#define DATA_A {0xad, 0xbe, 0xde}
#define DATA_B {0xde, 0xef, 0xf0}

namespace Svc {

class ComRetryTester final : public ComRetryGTestBase {
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

    //! Construct object ComRetryTester
    ComRetryTester();

    //! Destroy object ComRetryTester
    ~ComRetryTester();

  public:
    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------
    void configure(U32 num_retries);

    void receiveBuffer(Fw::Buffer &buffer, ComCfg::FrameContext &context);

    void checkDataOut(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedDataSize);

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testBufferSend();

    void testBufferRetry();

    void testBufferRetryTillFailure();

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
    ComRetry component;
};

}  // namespace Svc

#endif
