// ======================================================================
// \title  FrameAccumulatorTester.cpp
// \author chammard
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
    this->initComponents();
    this->connectPorts();
    component.configure(this->mockDetector, 1, this->mallocator, 2048);
}

FrameAccumulatorTester ::~FrameAccumulatorTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FrameAccumulatorTester ::testFrameDetected() {
    // Prepare a random size buffer
    U32 buffer_size = STest::Random::lowerUpper(1, 1024);
    U8 data[buffer_size];
    Fw::Buffer buffer(data, buffer_size);
    // Set the mock detector to report success of size_out = buffer_size
    this->mockDetector.next_status = FrameDetector::Status::FRAME_DETECTED;
    this->mockDetector.next_size_out = buffer_size;
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_dataDeallocate_SIZE(1); // input buffer was deallocated
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
    this->mockDetector.next_status = FrameDetector::Status::MORE_DATA_NEEDED;
    this->mockDetector.next_size_out = buffer_size + 1;
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_dataDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(0); // frame was not sent (waiting on more data)
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), buffer_size); // data left in ring buffer
}

void FrameAccumulatorTester ::testNoFrameDetected() {
    // Prepare a random size buffer
    U32 buffer_size = STest::Random::lowerUpper(1, 1024);
    U8 data[buffer_size];
    Fw::Buffer buffer(data, buffer_size);
    // Set the mock detector
    this->mockDetector.next_status = FrameDetector::Status::NO_FRAME_DETECTED;
    this->mockDetector.next_size_out = 0;
    // Receive the buffer on dataIn
    this->invoke_to_dataIn(0, buffer, Drv::RecvStatus::RECV_OK);
    // Checks
    ASSERT_from_dataDeallocate_SIZE(1); // input buffer was deallocated
    ASSERT_from_frameOut_SIZE(0); // No frame was sent out
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0); // all data was consumed and discarded
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// Port handler overrides
// ----------------------------------------------------------------------
Fw::Buffer FrameAccumulatorTester ::from_frameAllocate_handler(
        FwIndexType portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_frameAllocate(size);
    this->m_buffer.setData(this->m_buffer_slot);
    this->m_buffer.setSize(size);
    ::memset(this->m_buffer.getData(), 0, size);
    return this->m_buffer;
  }

}  // namespace Svc
