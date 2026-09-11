// ======================================================================
// \title  ComAggregatorTester.hpp
// \author lestarch
// \brief  hpp file for ComAggregator component test harness implementation class
// ======================================================================

#ifndef Svc_ComAggregatorTester_HPP
#define Svc_ComAggregatorTester_HPP

#include <deque>
#include <vector>
#include "Fw/Types/MallocAllocator.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/ComAggregator/ComAggregator.hpp"
#include "Svc/ComAggregator/ComAggregatorGTestBase.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

//! Allocator delegating to malloc while recording calls, and optionally failing, for lifecycle tests
class CountingAllocator final : public Fw::MemAllocator {
  public:
    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment = alignof(std::max_align_t)) override;

    void deallocate(const FwEnumStoreType identifier, void* ptr) override;

    Fw::MallocAllocator m_delegate;
    //! Number of allocate() calls
    U32 m_allocations = 0;
    //! Number of deallocate() calls
    U32 m_deallocations = 0;
    //! Identifier seen on the last allocate()/deallocate() call
    FwEnumStoreType m_lastId = -1;
    //! Pointer handed out by the last allocate() call
    void* m_lastPointer = nullptr;
    //! When set, allocate() returns nullptr
    bool m_failAllocation = false;
    //! When non-zero, allocate() reports this many bytes fewer than requested
    FwSizeType m_shortfall = 0;
};

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

    // Aggregation size used unless a test supplies its own: the TM transfer frame data field
    static constexpr FwSizeType DEFAULT_AGGREGATION_SIZE = static_cast<FwSizeType>(Ccsds::TmDataFieldSize);

    // Allocation identifier supplied to the component instance under test
    static constexpr FwEnumStoreType TEST_ALLOCATION_ID = 3;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComAggregatorTester, configuring the component under test
    ComAggregatorTester(FwSizeType aggregationSize = DEFAULT_AGGREGATION_SIZE, bool spanning = false);

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

    //! Tests that a packet leaving less than a minimum idle packet of residual is held for the next aggregate
    void test_small_residual_holds();

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

    //! Tests that configure() asserts on aggregation sizes outside the supported range
    void test_configure_invalid_size_asserts();

    //! Tests that configure() asserts when the allocator fails to provide the aggregation buffer
    void test_configure_allocation_failure_asserts();

    //! Tests that cleanup() releases the aggregation buffer exactly once and tolerates repeated calls
    void test_cleanup();

    //! dataIn on a released instance asserts
    void test_datain_after_cleanup_asserts();

    //! Tests that, without spanning, a held packet larger than an aggregate asserts
    void test_oversize_hold_asserts();

    //! Tests that, without spanning, an oversized input asserts in the input handler
    void test_oversize_fill_asserts();

    //! Tests that a lost frame drops the remainder of a split packet
    void test_spanning_failure_drops_split_remainder();

    //! Helper to fill a buffer with random data
    Fw::Buffer fill_buffer(U32 size);

    //! Shadow aggregate a buffer for validation
    void shadow_aggregate(const Fw::Buffer& buffer);

    //! Validate against shadow aggregation: aggregated packets followed by idle fill to the aggregation size
    void validate_aggregation(const Fw::Buffer& buffer);

    //! Validate an emitted aggregate against the shadow aggregation with a First Header Pointer of 0
    void validate_emitted_aggregation(U32 index);

    //! Configured aggregation size of the component under test
    FwSizeType aggregation_size() const;

    //! Largest packet accepted without spanning: leaves room for a minimum idle packet in an empty aggregate
    FwSizeType max_packet_size() const;

    //! Send a buffer that must be aggregated (not held) and validate it against the shadow aggregation
    void fill_with(U32 size);

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
    //! Allocator supplying the aggregation buffer
    CountingAllocator m_allocator;
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
    //! Spanning shadow: the outstanding aggregate was reported lost
    bool m_failurePending = false;
    //! Spanning shadow: a packet is retained by the component pending full consumption
    bool m_heldPending = false;
    //! Spanning shadow: the retained packet has bytes in an emitted aggregate
    bool m_heldConsumed = false;
    //! Spanning shadow: number of carried idle bytes at the aggregate start
    FwSizeType m_leadingIdleCount = 0;
    //! Spanning shadow: the last emitted aggregate, for return
    Fw::Buffer m_outFrame;
    //! Spanning shadow: the context of the last emitted aggregate
    ComCfg::FrameContext m_outContext;
};

}  // namespace Svc

#endif
