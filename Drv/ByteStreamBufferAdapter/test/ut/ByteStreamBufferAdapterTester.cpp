// ======================================================================
// \title  ByteStreamBufferAdapterTester.cpp
// \author shahab
// \brief  cpp file for ByteStreamBufferAdapter component test harness implementation class
// ======================================================================

#include "ByteStreamBufferAdapterTester.hpp"
#include <STest/Pick/Pick.hpp>

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ByteStreamBufferAdapterTester ::ByteStreamBufferAdapterTester()
    : ByteStreamBufferAdapterGTestBase("ByteStreamBufferAdapterTester",
                                       ByteStreamBufferAdapterTester::MAX_HISTORY_SIZE),
      component("ByteStreamBufferAdapter"),
      m_buffer(m_data_store, DATA_SIZE),
      m_byte_stream_driver_fail(false) {
    this->initComponents();
    this->connectPorts();
}

ByteStreamBufferAdapterTester ::~ByteStreamBufferAdapterTester() {}

// --------------------------------
// Overridden functions
// --------------------------------
Drv::ByteStreamStatus ByteStreamBufferAdapterTester ::from_toByteStreamDriver_handler(FwIndexType portNum,
                                                                                      Fw::Buffer& sendBuffer) {
    this->pushFromPortEntry_toByteStreamDriver(sendBuffer);
    if (this->m_byte_stream_driver_fail) {
        return Drv::ByteStreamStatus::OTHER_ERROR;
    } else {
        return Drv::ByteStreamStatus::OP_OK;
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ByteStreamBufferAdapterTester ::random_fill(Fw::SerializeBufferBase& buffer, U32 max_size) {
    U32 random_size = STest::Pick::lowerUpper(0, max_size);
    // buffer.resetSer();
    for (U32 i = 0; i < random_size; i++) {
        buffer.serializeFrom(static_cast<U8>(STest::Pick::any()));
    }
}

void ByteStreamBufferAdapterTester ::test_byte_stream_out() {
    clearFromPortHistory();
    this->clearHistory();

    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);

    invoke_to_bufferIn(0, m_buffer);

    // driver is not ready by default, verify no data flow
    ASSERT_from_toByteStreamDriver_SIZE(0);
    ASSERT_EVENTS_DriverNotReady_SIZE(1);
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_bufferInReturn(0, m_buffer);

    // driver ready, verify data is flowing
    invoke_to_byteStreamDriverReady(0);

    // Clear histories before next test
    clearFromPortHistory();
    this->clearHistory();

    invoke_to_bufferIn(0, m_buffer);
    ASSERT_from_toByteStreamDriver_SIZE(1);
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_toByteStreamDriver(0, m_buffer);
    // No DriverNotReady event should be issued since driver is now ready
    ASSERT_EVENTS_DriverNotReady_SIZE(0);

    // Clear histories before testing error case
    clearFromPortHistory();
    this->clearHistory();

    // Force data failure
    this->m_byte_stream_driver_fail = true;
    invoke_to_bufferIn(0, m_buffer);

    // Verify data send error
    ASSERT_EVENTS_DataSendError_SIZE(1);
    // Should still see ByteStreamStatus::OTHER_ERROR (3) as error code
    ASSERT_EVENTS_DataSendError(0, 3);
    // Should not see any DriverNotReady events since the driver is ready
    ASSERT_EVENTS_DriverNotReady_SIZE(0);
    ASSERT_from_toByteStreamDriver_SIZE(1);
    ASSERT_from_bufferInReturn_SIZE(1);
    this->m_byte_stream_driver_fail = false;
}

void ByteStreamBufferAdapterTester ::test_byte_stream_in() {
    clearFromPortHistory();
    this->clearHistory();

    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);

    // Set driver to ready state first
    invoke_to_byteStreamDriverReady(0);

    // Faulty data from byte stream driver; send with bad status
    invoke_to_fromByteStreamDriver(0, m_buffer, Drv::ByteStreamStatus::RECV_NO_DATA);

    // Verify no data at buffer out as the status was bad
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_EVENTS_DataReceiveError_SIZE(1);
    // Verify buffer was returned immediately
    ASSERT_from_fromByteStreamDriverReturn_SIZE(1);
    ASSERT_from_fromByteStreamDriverReturn(0, m_buffer);

    // Clear histories before good data test
    clearFromPortHistory();
    this->clearHistory();

    // Good data
    invoke_to_fromByteStreamDriver(0, m_buffer, Drv::ByteStreamStatus::OP_OK);

    // Verify no errors and data is sent out
    ASSERT_EVENTS_DataReceiveError_SIZE(0);
    ASSERT_from_bufferOut_SIZE(1);
    ASSERT_from_bufferOut(0, m_buffer);
    // Verify buffer was NOT returned (since it's passed to client)
    ASSERT_from_fromByteStreamDriverReturn_SIZE(0);
}

void ByteStreamBufferAdapterTester ::test_byte_stream_return() {
    clearFromPortHistory();
    this->clearHistory();

    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);

    invoke_to_bufferOutReturn(0, m_buffer);

    // Must return buffer regardless of driver ready state
    ASSERT_from_fromByteStreamDriverReturn_SIZE(1);
    ASSERT_from_fromByteStreamDriverReturn(0, m_buffer);
}

void ByteStreamBufferAdapterTester ::test_driver_ready_state() {
    // Clear all histories
    clearFromPortHistory();
    this->clearHistory();

    // Create a test buffer
    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);

    // Test 1: Initial state - Driver should not be ready
    // Try to send data
    invoke_to_bufferIn(0, m_buffer);

    // Verify no data was sent to the driver
    ASSERT_from_toByteStreamDriver_SIZE(0);
    // Verify a DriverNotReady event was issued
    ASSERT_EVENTS_DriverNotReady_SIZE(1);
    // Verify buffer was returned regardless
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_bufferInReturn(0, m_buffer);

    // Clear histories for next test
    clearFromPortHistory();
    this->clearHistory();

    // Test 2: Set driver to ready state
    invoke_to_byteStreamDriverReady(0);

    // Try to send data again
    invoke_to_bufferIn(0, m_buffer);

    // Now data should flow to driver
    ASSERT_from_toByteStreamDriver_SIZE(1);
    // No DriverNotReady event should be issued
    ASSERT_EVENTS_DriverNotReady_SIZE(0);
    // Buffer should still be returned
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_bufferInReturn(0, m_buffer);
}

}  // namespace Drv
