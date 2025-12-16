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
// Overriden functions
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

//! Handle a text event
void ByteStreamBufferAdapterTester ::textLogIn(FwEventIdType id,                //!< The event ID
                                               const Fw::Time& timeTag,         //!< The time
                                               const Fw::LogSeverity severity,  //!< The severity
                                               const Fw::TextLogString& text    //!< The event string
) {
    TextLogEntry e = {id, timeTag, severity, text};

    printTextLogHistoryEntry(e, stdout);
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

    // driver ready, verify data is flowing
    invoke_to_byteStreamDriverReady(0);
    invoke_to_bufferIn(0, m_buffer);
    ASSERT_from_toByteStreamDriver_SIZE(1);
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_toByteStreamDriver(0, m_buffer);

    // Force data failure
    this->m_byte_stream_driver_fail = true;
    invoke_to_bufferIn(0, m_buffer);

    // Verify data send error
    ASSERT_EVENTS_DataSendError_SIZE(1);
    ASSERT_EVENTS_DataSendError(0, 3);
    ASSERT_EVENTS_DriverNotReady_SIZE(1);
    ASSERT_from_toByteStreamDriver_SIZE(2);
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

    invoke_to_byteStreamDriverReady(0);

    // Faulty data from byte stream driver; send with bad status
    invoke_to_fromByteStreamDriver(0, m_buffer, Drv::ByteStreamStatus::RECV_NO_DATA);

    // Verify no data at buffer out as the status was bad
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_EVENTS_DataReceiveError_SIZE(1);

    // Good data
    this->clearHistory();
    invoke_to_fromByteStreamDriver(0, m_buffer, Drv::ByteStreamStatus::OP_OK);

    // Verify no errors and data is sent out
    ASSERT_EVENTS_DataReceiveError_SIZE(0);
    ASSERT_from_bufferOut_SIZE(1);
    ASSERT_from_bufferOut(0, m_buffer);
}

void ByteStreamBufferAdapterTester ::test_byte_stream_return() {
    clearFromPortHistory();

    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);

    invoke_to_byteStreamDriverReady(0);
    invoke_to_bufferOutReturn(0, m_buffer);

    // **must** return buffer
    ASSERT_from_fromByteStreamDriverReturn_SIZE(1);
    ASSERT_from_fromByteStreamDriverReturn(0, m_buffer);
}

}  // namespace Drv
