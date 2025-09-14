// ======================================================================
// \title  FrameAccumulatorTester.hpp
// \author thomas-bc
// \brief  hpp file for FrameAccumulator component test harness implementation class
// ======================================================================

#ifndef Svc_FrameAccumulatorTester_HPP
#define Svc_FrameAccumulatorTester_HPP

#include "Fw/Types/MallocAllocator.hpp"
#include "Svc/FrameAccumulator/FrameAccumulator.hpp"
#include "Svc/FrameAccumulator/FrameAccumulatorGTestBase.hpp"
#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"

namespace Svc {

class FrameAccumulatorTester : public FrameAccumulatorGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 200;

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

    //! Test that frame is detected
    void testFrameDetected();

    //! More data needed
    void testMoreDataNeeded();

    //! No frame detected
    void testNoFrameDetected();

    //! Receive a zero size buffer
    void testReceiveZeroSizeBuffer();

    // Send two buffers successively and check that the frame is correctly accumulated
    void testAccumulateTwoBuffers();

    //! Test accumulation of multiple random-size buffer into a frame
    void testAccumulateBuffersEmitFrame();

    //! Test accumulation of multiple random-size buffer into frames successively
    void testAccumulateBuffersEmitManyFrames();

    //! Test returning ownership of a buffer
    void testBufferReturnDeallocation();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Send a series of random-size buffers, terminated by a buffer that
    //! will be detected as a full frame by the MockDetector
    //! (output) frame_size and buffer_count are updated with the size of the frame and the number of buffers sent
    void mockAccumulateFullFrame(U32& frame_size, U32& buffer_count);

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Port handler overrides
    // ----------------------------------------------------------------------
    //! Overriding bufferAllocate handler to be able to request a buffer in component tests
    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    //! MockDetector is used to control the behavior of the component under test
    //! by controlling what the FrameDetector will report without needing to craft
    //! legitimate frames from specific to a protocol
    class MockDetector : public FrameDetector {
      public:
        Status detect(const Types::CircularBuffer& data, FwSizeType& size_out) const override {
            size_out = this->next_size_out;
            return next_status;
        }

        void set_next_result(Status status, FwSizeType size_out) {
            this->next_size_out = size_out;
            this->next_status = status;
        }

        Status next_status = Status::FRAME_DETECTED;
        FwSizeType next_size_out = 0;
    };

    //! Instances required by the component under test
    MockDetector mockDetector;
    Fw::MallocAllocator mallocator;

    //! The component under test (should be listed after mallocator for safe destruction)
    Svc::FrameAccumulator component;

    Fw::Buffer m_buffer;  // buffer to be returned by mocked bufferAllocate call
    U8 m_buffer_slot[2048];
};

}  // namespace Svc

#endif
