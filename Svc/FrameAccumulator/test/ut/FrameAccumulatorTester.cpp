// ======================================================================
// \title  FrameAccumulatorTester.cpp
// \author thomas-bc
// \brief  cpp file for FrameAccumulator component test harness implementation class
// ======================================================================

#include "FrameAccumulatorTester.hpp"
#include "STest/Random/Random.hpp"



namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FrameAccumulatorTester ::FrameAccumulatorTester()
    : FrameAccumulatorGTestBase("FrameAccumulatorTester", FrameAccumulatorTester::MAX_HISTORY_SIZE),
      component("FrameAccumulator") {
    component.configure(this->mockDetector, 1, this->mallocator, 2048);
    this->initComponents();
    this->connectPorts();
}


FrameAccumulatorTester ::~FrameAccumulatorTester() {
    component.cleanup();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FrameAccumulatorTester ::testFrameDetected() {
    // Prepare a random size buffer
    U32 buffer_size = STest::Random::lowerUpper(1, 1024);
    U8 data[buffer_size];
    Fw::Buffer buffer(data, buffer_size);
    // Set the mock detector to report success of size_out = buffer_size
    this->mockDetector.set_next_result(FrameDetector::Status::FRAME_DETECTED, buffer_size);
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_bufferDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(1); // frame was sent
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // no data left in ring buffer
    ASSERT_EQ(this->fromPortHistory_frameOut->at(0).data.getSize(), buffer_size); // all data was sent out
}

void FrameAccumulatorTester ::testMoreDataNeeded() {
    // Prepare a random size buffer
    U32 buffer_size = STest::Random::lowerUpper(1, 1024);
    U8 data[buffer_size];
    Fw::Buffer buffer(data, buffer_size);
    // Set the mock detector to report more data needed
    this->mockDetector.set_next_result(FrameDetector::Status::MORE_DATA_NEEDED, buffer_size + 1);
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_bufferDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(0); // frame was not sent (waiting on more data)
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), buffer_size); // data left in ring buffer
}

void FrameAccumulatorTester ::testNoFrameDetected() {
    // Prepare a random size buffer
    U32 buffer_size = STest::Random::lowerUpper(1, 1024);
    U8 data[buffer_size];
    Fw::Buffer buffer(data, buffer_size);
    // Set the mock detector
    this->mockDetector.set_next_result(FrameDetector::Status::NO_FRAME_DETECTED, 0);
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_bufferDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(0); // No frame was sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // all data was consumed and discarded
}

void FrameAccumulatorTester ::testReceiveZeroSizeBuffer() {
    
    // Prepare a zero size buffer
    U8 data[1] = {0};
    Fw::Buffer buffer(data, 0);
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_bufferDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(0); // No frame was sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // No data in ring buffer
    ASSERT_EQ(this->component.m_inRing.m_head_idx, 0);
}

void FrameAccumulatorTester ::testAccumulateTwoBuffers() {
    Fw::Buffer::SizeType buffer1_size = 10;
    Fw::Buffer::SizeType buffer2_size = 20;
    U8 data1[buffer1_size];
    U8 data2[buffer2_size];
    Fw::Buffer buffer1(data1, buffer1_size);
    Fw::Buffer buffer2(data2, buffer2_size);

    this->mockDetector.set_next_result(FrameDetector::Status::MORE_DATA_NEEDED, buffer2_size);
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer1, Drv::RecvStatus::RECV_OK);
    // Next result is detection of a full frame, size = buffer1_size + buffer2_size
    this->mockDetector.set_next_result(FrameDetector::Status::FRAME_DETECTED, buffer1_size + buffer2_size );
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer2, Drv::RecvStatus::RECV_OK);

    // Checks
    ASSERT_from_bufferDeallocate_SIZE(2); // both input buffers deallocated
    ASSERT_from_frameOut_SIZE(1); // Exactly one frame was sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // No data in ring buffer
}

void FrameAccumulatorTester ::testAccumulateBuffersEmitFrame() {
    U32 frame_size = 0;
    U32 buffer_count = 0;
    this->mockAccumulateFullFrame(frame_size, buffer_count);
    // Checks
    ASSERT_from_bufferDeallocate_SIZE(buffer_count); // all input buffers deallocated
    ASSERT_from_frameOut_SIZE(1); // Exactly one frame was sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // No data left in ring buffer
    ASSERT_EQ(this->fromPortHistory_frameOut->at(0).data.getSize(), frame_size); // accumulated buffer size
}

void FrameAccumulatorTester ::testAccumulateBuffersEmitManyFrames() {
    U32 max_iters = STest::Random::lowerUpper(1, 10);
    U32 total_buffer_received = 0;

    U32 frame_size = 0;
    U32 buffer_count = 0;

    // Send frames successively, perform some checks after each frame
    for (U32 i = 0; i < max_iters; i++) {
        this->mockAccumulateFullFrame(frame_size, buffer_count);
        total_buffer_received += buffer_count;

        ASSERT_from_bufferDeallocate_SIZE(total_buffer_received); // all input buffers deallocated
        ASSERT_from_frameOut_SIZE(i+1); // Exactly one frame was sent out
        ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // No data left in ring buffer
        ASSERT_EQ(this->fromPortHistory_frameOut->at(i).data.getSize(), frame_size); // accumulated buffer size
    }
    // Final checks
    ASSERT_from_bufferDeallocate_SIZE(total_buffer_received); // all input buffers deallocated
    ASSERT_from_frameOut_SIZE(max_iters); // Exactly max_iters frames were sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // No data left in ring buffer
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void FrameAccumulatorTester ::mockAccumulateFullFrame(U32& frame_size, U32& buffer_count) {
    // Constants need to be picked so that:
    // - the worst case doesn't overflow the ring buffer size (max_size * iters < 2048)
    // - iters < MAX_HISTORY_SIZE
    const U32 buffer_max_size = 64;
    const U32 iters = STest::Random::lowerUpper(0, 10);

    U8 data[buffer_max_size];
    U32 buffer_size;
    Fw::Buffer buffer(data, 0);
    U32 accumulated_size = 0;

    // Send multiple buffers with MORE_DATA_NEEDED
    for (U32 i = 0; i < iters; i++) {
        buffer_size = STest::Random::lowerUpper(1, buffer_max_size);
        accumulated_size += buffer_size;
        buffer.setSize(buffer_size);
        // Detector reports MORE_DATA_NEEDED and size needed bigger than accumulated size so far
        this->mockDetector.set_next_result(FrameDetector::Status::MORE_DATA_NEEDED, accumulated_size + 1);
        this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    }

    // Send last buffer with FRAME_DETECTED
    buffer_size = STest::Random::lowerUpper(1, buffer_max_size);
    buffer.setSize(buffer_size);
    accumulated_size += buffer_size; // accumulate once more (sending last buffer below)
    // Send last buffer with finally FRAME_DETECTED and total accumulated + last buffer
    this->mockDetector.set_next_result(FrameDetector::Status::FRAME_DETECTED, accumulated_size);
    // Receive the last buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    frame_size = accumulated_size;
    buffer_count = iters + 1;
}

// ----------------------------------------------------------------------
// Port handler overrides
// ----------------------------------------------------------------------
Fw::Buffer FrameAccumulatorTester ::from_bufferAllocate_handler(
        FwIndexType portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_bufferAllocate(size);
    this->m_buffer.setData(this->m_buffer_slot);
    this->m_buffer.setSize(size);
    ::memset(this->m_buffer.getData(), 0, size);
    return this->m_buffer;
  }

}  // namespace Svc
