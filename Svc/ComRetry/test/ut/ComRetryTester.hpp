// ======================================================================
// \title  ComRetryTester.hpp
// \author valdaarhun
// \brief  hpp file for ComRetry test harness implementation class
// ======================================================================

#ifndef Svc_ComRetryTester_HPP
#define Svc_ComRetryTester_HPP

#include "ComRetryGTestBase.hpp"
#include "Fw/Test/UnitTestAssert.hpp"
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
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test: the minimum depth documented in the SDD
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 4;

    // Maximum number of statuses the emulated synchronous adapter can be scripted with
    static const FwSizeType MAX_INLINE_STATUSES = 8;

    //! Kinds of output port invocations, used to check output ordering
    enum OutputKind { DATA_OUT, DATA_RETURN_OUT, COM_STATUS_OUT, PING_OUT };

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

    //! Send a buffer and return its ownership, dispatching each message
    void receiveBuffer(Fw::Buffer& buffer, ComCfg::FrameContext& context);

    //! Send a status and dispatch it
    void sendStatus(Fw::Success status);

    //! Dispatch every message on the component queue
    void dispatchAll();

    //! Number of messages waiting on the component queue
    FwSizeType queuedMessages();

    //! Emulate an adapter that answers synchronously from within dataOut, using the given statuses in order
    void setInlineAdapter(const Fw::Success* statuses, FwSizeType count);

    void checkDataOut(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedDataSize);

    void checkOutputOrder(const OutputKind* expected, FwSizeType count);

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testNullBuffer();

    void testBufferSend();

    void testBufferRetry();

    void testBufferRetryTillFailure();

    void testInputsQueued();

    void testSynchronousAdapterRetry();

    void testSynchronousAdapterExhaustion();

    void testPing();

    void testQueueFullAsserts();

    void testPingDroppedWhenFull();

    //! Test that no retries are attempted when configured with zero retries
    void testNoRetries();

    //! Test that the thread preamble asserts on an undersized queue
    void testQueueDepthCheck();

  private:
    //! Check that a one-argument FW_ASSERT with the expected argument was raised, then clear it
    void checkAssert(::Test::UnitTestAssert& assertHook, FwAssertArgType expectedArg1);

  public:
  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    void from_dataOut_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) override;

    void from_dataReturnOut_handler(FwIndexType portNum,
                                    Fw::Buffer& data,
                                    const ComCfg::FrameContext& context) override;

    void from_comStatusOut_handler(FwIndexType portNum, Fw::Success& condition) override;

    void from_pingOut_handler(FwIndexType portNum, U32 key) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Record an output invocation for ordering checks
    void recordOutput(OutputKind kind);

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

    //! Statuses the emulated synchronous adapter answers with
    Fw::Success m_inlineStatuses[MAX_INLINE_STATUSES];
    FwSizeType m_inlineCount;
    FwSizeType m_inlineIndex;

    //! Ordered record of output invocations
    OutputKind m_outputOrder[MAX_HISTORY_SIZE];
    FwSizeType m_outputCount;
};

}  // namespace Svc

#endif
