// ======================================================================
// \title  FrameAccumulatorTester.hpp
// \author chammard
// \brief  hpp file for FrameAccumulator component test harness implementation class
// ======================================================================

#ifndef Svc_FrameAccumulatorTester_HPP
#define Svc_FrameAccumulatorTester_HPP

#include "Svc/FrameAccumulator/FrameAccumulator.hpp"
#include "Svc/FrameAccumulator/FrameAccumulatorGTestBase.hpp"
#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"
#include "Fw/Types/MallocAllocator.hpp"

namespace Svc {

class FrameAccumulatorTester : public FrameAccumulatorGTestBase {
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

    //! Construct object FrameAccumulatorTester
    FrameAccumulatorTester();

    //! Destroy object FrameAccumulatorTester
    ~FrameAccumulatorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! 
    void testFrameDetected();
    void testMoreDataNeeded();
    void testNoFrameDetected();

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
    // Port handler overrides
    // ----------------------------------------------------------------------
    Fw::Buffer from_frameAllocate_handler(FwIndexType portNum, U32 size) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FrameAccumulator component;

    class MockDetector : public FrameDetector {
      public:
        Status detect(const Types::CircularBuffer& data, FwSizeType& size_out) const override {
            size_out = next_size_out;
            return next_status;
        }

        Status next_status = Status::FRAME_DETECTED;
        U32 next_size_out = 0;
    };

    //! Instances required by the component under test
    MockDetector mockDetector;
    Fw::MallocAllocator mallocator;

    Fw::Buffer m_buffer; // buffer to be returned by mocked frameAllocate call
    U8 m_buffer_slot[2048];
};

}  // namespace Svc

#endif
