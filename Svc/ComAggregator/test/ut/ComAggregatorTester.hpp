// ======================================================================
// \title  ComAggregatorTester.hpp
// \author lestarch
// \brief  hpp file for ComAggregator component test harness implementation class
// ======================================================================

#ifndef Svc_ComAggregatorTester_HPP
#define Svc_ComAggregatorTester_HPP

#include <vector>
#include "Svc/ComAggregator/ComAggregator.hpp"
#include "Svc/ComAggregator/ComAggregatorGTestBase.hpp"

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

    //! Helper to fill a buffer with random data
    Fw::Buffer fill_buffer(U32 size);

    //! Shadow aggregate a buffer for validation
    void shadow_aggregate(const Fw::Buffer& buffer);

    //! Validate against shadow aggregation
    void validate_aggregation(const Fw::Buffer& buffer);

    //! Helper to validate a buffer has been aggregated correctly
    void validate_buffer_aggregated(const Fw::Buffer& buffer, const ComCfg::FrameContext& context);

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
};

}  // namespace Svc

#endif
