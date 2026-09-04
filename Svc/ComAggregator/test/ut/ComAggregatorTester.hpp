// ======================================================================
// \title  ComAggregatorTester.hpp
// \author lestarch
// \brief  hpp file for ComAggregator component test harness implementation class
// ======================================================================

#ifndef Svc_ComAggregatorTester_HPP
#define Svc_ComAggregatorTester_HPP

#include <deque>
#include <vector>
#include "Svc/ComAggregator/ComAggregator.hpp"
#include "Svc/ComAggregator/ComAggregatorGTestBase.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

class ComAggregatorTester final : public ComAggregatorGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 20;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComAggregatorTester
    ComAggregatorTester();

    //! Destroy object ComAggregatorTester
    ~ComAggregatorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Tests initial operation
    void test_initial();

    //! Tests fill operation
    Fw::Buffer test_fill(bool expect_hold = false);

    //! Tests fill operation
    void test_fill_multi();

    //! Tests full operation
    void test_full();

    //! Tests exactly full operation
    void test_exactly_full();

    //! Tests timeout operation
    void test_timeout();

    //! Tests timeout operation
    void test_timeout_overflow_prevention();

    //! Tests timeout operation sends no empty buffer
    void test_timeout_zero();

    //! Tests hold while waiting on data return
    void test_hold_while_waiting();

    //! Tests clear operation
    void test_clear();

    //! Tests clear operation with held data
    void test_clear_with_hold();

    //! Tests a packet spanning from one aggregate into a single following aggregate
    void test_spanning_split_two();

    //! Tests a packet spanning a start aggregate, a complete middle aggregate, and an end aggregate
    void test_spanning_three_frames();

    //! Tests an idle packet spanning across aggregates when residual space is below the minimum
    void test_spanning_idle_span();

    //! Tests that configure() asserts once data has been aggregated
    void test_configure_after_fill_asserts();

    //! Tests that, without spanning, a held packet larger than an aggregate asserts
    void test_oversize_hold_asserts();

    //! Helper to fill a buffer with random data
    Fw::Buffer fill_buffer(U32 size);

    //! Shadow aggregate a buffer for validation
    void shadow_aggregate(const Fw::Buffer& buffer);

    //! Validate against shadow aggregation
    void validate_aggregation(const Fw::Buffer& buffer);

    //! Helper to validate a buffer has been aggregated correctly
    void validate_buffer_aggregated(const Fw::Buffer& buffer, const ComCfg::FrameContext& context);

    //! Helper to append the expected encoding of an SPP idle packet of the given total size
    static void append_idle_packet(std::vector<U8>& expected, FwSizeType idleSize);

    //! Helper to send a data buffer and dispatch the state machine
    void spanning_send(Fw::Buffer& buffer);

    //! Helper to validate an emitted aggregate against expected content and First Header Pointer
    void expect_frame(U32 index, const std::vector<U8>& expected, U16 expectedFhp);

    //! Helper to return the emitted aggregate and send a SUCCESS status
    void return_and_status(U32 index);

  public:
    // ----------------------------------------------------------------------
    // Spanning rules (rule-based testing)
    //
    // Shadow model: the aggregator emits a byte stream of packets and idle fill,
    // cut into capacity-sized aggregates. m_stream holds the bytes not yet emitted.
    // ----------------------------------------------------------------------

    //! Send a packet while filling: fits, exactly fills, or splits into the next aggregate
    FW_RBT_DEFINE_RULE(ComAggregatorTester, Spanning, SendPacket);

    //! Send a packet while an aggregate is outstanding: packet is held whole
    FW_RBT_DEFINE_RULE(ComAggregatorTester, Spanning, SendPacketWhileWaiting);

    //! Timeout while filling: residual space is idle-filled, spanning the idle packet if needed
    FW_RBT_DEFINE_RULE(ComAggregatorTester, Spanning, Timeout);

    //! Failure status while an aggregate is outstanding: nothing changes
    FW_RBT_DEFINE_RULE(ComAggregatorTester, Spanning, StatusFailure);

    //! Return the outstanding aggregate with SUCCESS: held data refills, possibly a full aggregate
    FW_RBT_DEFINE_RULE(ComAggregatorTester, Spanning, ReturnAndStatus);

    //! Enable spanning and run the initial status handshake
    void spanning_rbt_start();

    //! Release any packets not yet returned
    void spanning_rbt_finish();

  private:
    //! Append a random packet to the shadow stream and send it to the component
    void shadow_send_packet();

    //! Validate the emitted aggregate against the head of the shadow stream and consume it
    void shadow_emit();

    //! Validate the oldest unreturned packet was returned and release it
    void shadow_expect_return(U32 index);

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
    ComAggregator component;
    //! Shadow aggregation for validation
    std::vector<U8> m_aggregation;

    //! Spanning shadow: bytes not yet emitted in an aggregate
    std::vector<U8> m_stream;
    //! Spanning shadow: offsets into m_stream where a packet header starts (ascending)
    std::vector<FwSizeType> m_headers;
    //! Spanning shadow: packets sent to the component and not yet returned, in order
    std::deque<U8*> m_unreturned;
    //! Spanning shadow: an aggregate has been emitted and awaits return and status
    bool m_outstanding = false;
    //! Spanning shadow: a packet is retained by the component pending full consumption
    bool m_heldPending = false;
    //! Spanning shadow: the last emitted aggregate, for return
    Fw::Buffer m_outFrame;
    //! Spanning shadow: the context of the last emitted aggregate
    ComCfg::FrameContext m_outContext;
};

}  // namespace Svc

#endif
