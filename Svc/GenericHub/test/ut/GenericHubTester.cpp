// ======================================================================
// \title  GenericHub.hpp
// \author mstarch
// \brief  cpp file for GenericHub test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GenericHubTester.hpp"
#include <STest/Pick/Pick.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10000

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

GenericHubTester ::GenericHubTester()
    : GenericHubGTestBase("Tester", MAX_HISTORY_SIZE),
      componentIn("GenericHubIn"),
      componentOut("GenericHubOut"),
      m_buffer(m_data_store, DATA_SIZE),
      m_comm_in(0),
      m_buffer_in(0),
      m_comm_out(0),
      m_buffer_out(0),
      m_current_port(0) {
    this->initComponents();
    this->connectPorts();
}

GenericHubTester ::~GenericHubTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void GenericHubTester ::test_in_out() {
    U32 max = std::min(this->componentIn.getNum_portIn_InputPorts(), this->componentOut.getNum_portOut_OutputPorts());
    for (U32 i = 0; i < max; i++) {
        send_random_comm(i);
        ASSERT_from_dataInDeallocate_SIZE(1);
        fromPortHistory_dataInDeallocate->clear();
    }
}

void GenericHubTester ::test_buffer_io() {
    U32 max =
        std::min(this->componentIn.getNum_buffersIn_InputPorts(), this->componentOut.getNum_buffersOut_OutputPorts());
    for (U32 i = 0; i < max; i++) {
        send_random_buffer(i);
        ASSERT_from_dataInDeallocate_SIZE(1);
        fromPortHistory_dataInDeallocate->clear();
    }
}

void GenericHubTester ::test_random_io() {
    for (U32 i = 0; i < 10000; i++) {
        U32 choice = STest::Pick::lowerUpper(0, 1);
        if (choice) {
            U32 port = STest::Pick::lowerUpper(0, std::min(this->componentIn.getNum_portIn_InputPorts(),
                                                           this->componentOut.getNum_portOut_OutputPorts()) -
                                                      1);
            send_random_comm(port);
        } else {
            U32 port = STest::Pick::lowerUpper(0, std::min(this->componentIn.getNum_buffersIn_InputPorts(),
                                                           this->componentOut.getNum_buffersOut_OutputPorts()) -
                                                      1);
            send_random_buffer(port);
        }
        ASSERT_from_dataInDeallocate_SIZE(1);
        fromPortHistory_dataInDeallocate->clear();
    }
}

void GenericHubTester ::random_fill(Fw::SerializeBufferBase& buffer, U32 max_size) {
    U32 random_size = STest::Pick::lowerUpper(0, max_size);
    buffer.resetSer();
    for (U32 i = 0; i < random_size; i++) {
        buffer.serialize(static_cast<U8>(STest::Pick::any()));
    }
}

void GenericHubTester ::test_telemetry() {
    Fw::TlmBuffer buffer;
    random_fill(buffer, FW_TLM_BUFFER_MAX_SIZE);

    Fw::Time time(100, 200);
    invoke_to_TlmRecv(0, 123, time, buffer);

    // **must** deallocate buffer
    ASSERT_from_dataInDeallocate_SIZE(1);
    ASSERT_from_TlmSend_SIZE(1);
    ASSERT_from_TlmSend(0, 123, time, buffer);
    clearFromPortHistory();
}

void GenericHubTester ::test_events() {
    Fw::LogSeverity severity = Fw::LogSeverity::WARNING_HI;
    Fw::LogBuffer buffer;
    random_fill(buffer, FW_LOG_BUFFER_MAX_SIZE);

    Fw::Time time(100, 200);
    invoke_to_LogRecv(0, 123, time, severity, buffer);

    // **must** deallocate buffer
    ASSERT_from_dataInDeallocate_SIZE(1);
    ASSERT_from_LogSend_SIZE(1);
    ASSERT_from_LogSend(0, 123, time, severity, buffer);
    clearFromPortHistory();
}
// Helpers

void GenericHubTester ::send_random_comm(U32 port) {
    random_fill(m_comm, FW_COM_BUFFER_MAX_SIZE);
    m_current_port = port;
    invoke_to_portIn(m_current_port, m_comm);
    // Ensure that the data out was called, and that the portOut unwrapped properly
    ASSERT_from_dataOut_SIZE(m_comm_in + m_buffer_out + 1);
    ASSERT_EQ(m_comm_in + 1, m_comm_out);
    m_comm_in++;
}

void GenericHubTester ::send_random_buffer(U32 port) {
    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    random_fill(m_buffer.getSerializeRepr(), max_random_size);
    m_buffer.setSize(max_random_size);
    m_current_port = port;
    invoke_to_buffersIn(m_current_port, m_buffer);
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_from_bufferDeallocate(0, m_buffer);
    fromPortHistory_bufferDeallocate->clear();
    // Ensure that the data out was called, and that the portOut unwrapped properly
    ASSERT_from_dataOut_SIZE(m_buffer_in + m_comm_out + 1);
    ASSERT_EQ(m_buffer_in + 1, m_buffer_out);
    m_buffer_in++;
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void GenericHubTester ::from_LogSend_handler(const NATIVE_INT_TYPE portNum,
                                   FwEventIdType id,
                                   Fw::Time& timeTag,
                                   const Fw::LogSeverity& severity,
                                   Fw::LogBuffer& args) {
    this->pushFromPortEntry_LogSend(id, timeTag, severity, args);
}

void GenericHubTester ::from_TlmSend_handler(const NATIVE_INT_TYPE portNum,
                                   FwChanIdType id,
                                   Fw::Time& timeTag,
                                   Fw::TlmBuffer& val) {
    this->pushFromPortEntry_TlmSend(id, timeTag, val);
}

void GenericHubTester ::from_dataOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    ASSERT_NE(fwBuffer.getData(), nullptr) << "Empty buffer to deallocate";
    ASSERT_GE(fwBuffer.getData(), m_data_for_allocation) << "Incorrect data pointer deallocated";
    ASSERT_LT(fwBuffer.getData(), m_data_for_allocation + sizeof(m_data_for_allocation))
        << "Incorrect data pointer deallocated";
    // Reuse m_allocate to pass into the otherside of the hub
    this->pushFromPortEntry_dataOut(fwBuffer);
    invoke_to_dataIn(0, fwBuffer);
}

// ----------------------------------------------------------------------
// Handlers for serial from ports
// ----------------------------------------------------------------------

void GenericHubTester ::from_buffersOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    m_buffer_out++;
    // Assert the buffer came through exactly on the right port
    ASSERT_EQ(portNum, m_current_port);
    ASSERT_EQ(fwBuffer.getSize(), m_buffer.getSize());
    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        U8 byte1 = reinterpret_cast<U8*>(fwBuffer.getData())[i];
        U8 byte2 = reinterpret_cast<U8*>(m_buffer.getData())[i];
        ASSERT_EQ(byte1, byte2);
    }
    // Pretend to deallocate like file uplink would
    this->from_dataInDeallocate_handler(0, fwBuffer);
}

void GenericHubTester ::from_portOut_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                                   Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    m_comm_out++;
    // Assert the buffer came through exactly on the right port
    ASSERT_EQ(portNum, m_current_port);
    ASSERT_EQ(Buffer.getBuffLength(), m_comm.getBuffLength());
    for (U32 i = 0; i < Buffer.getBuffLength(); i++) {
        ASSERT_EQ(Buffer.getBuffAddr()[i], m_comm.getBuffAddr()[i]);
    }
    ASSERT_from_buffersOut_SIZE(0);
}

Fw::Buffer GenericHubTester ::from_dataOutAllocate_handler(const NATIVE_INT_TYPE portNum, const U32 size) {
    EXPECT_EQ(m_allocate.getData(), nullptr) << "Allocation buffer is still in use";
    EXPECT_LE(size, sizeof(m_data_for_allocation)) << "Allocation buffer is still in use";
    m_allocate.set(m_data_for_allocation, size);
    return m_allocate;
}

void GenericHubTester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // Check buffer deallocations here
    ASSERT_EQ(fwBuffer.getData(), m_buffer.getData()) << "Ensure that the buffer was deallocated";
    ASSERT_EQ(fwBuffer.getSize(), m_buffer.getSize()) << "Ensure that the buffer was deallocated";
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void GenericHubTester ::from_dataInDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    ASSERT_NE(fwBuffer.getData(), nullptr) << "Empty buffer to deallocate";
    ASSERT_GE(fwBuffer.getData(), m_data_for_allocation) << "Incorrect data pointer deallocated";
    ASSERT_LT(fwBuffer.getData(), m_data_for_allocation + sizeof(m_data_for_allocation))
        << "Incorrect data pointer deallocated";

    m_allocate.set(nullptr, 0);
    this->pushFromPortEntry_dataInDeallocate(fwBuffer);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void GenericHubTester ::connectPorts() {
    // buffersIn
    U32 max =
        std::min(this->componentIn.getNum_buffersIn_InputPorts(), this->componentOut.getNum_buffersOut_OutputPorts());
    for (U32 i = 0; i < max; ++i) {
        this->connect_to_buffersIn(i, this->componentIn.get_buffersIn_InputPort(i));
    }

    // LogRecv
    this->connect_to_LogRecv(0, this->componentIn.get_LogRecv_InputPort(0));

    // TlmRecv
    this->connect_to_TlmRecv(0, this->componentIn.get_TlmRecv_InputPort(0));

    // dataIn
    this->connect_to_dataIn(0, this->componentOut.get_dataIn_InputPort(0));

    // buffersOut
    for (U32 i = 0; i < max; ++i) {
        this->componentOut.set_buffersOut_OutputPort(i, this->get_from_buffersOut(i));
    }

    // LogSend
    this->componentOut.set_LogSend_OutputPort(0, this->get_from_LogSend(0));

    // TlmSend
    this->componentOut.set_TlmSend_OutputPort(0, this->get_from_TlmSend(0));

    // dataOut
    this->componentIn.set_dataOut_OutputPort(0, this->get_from_dataOut(0));

    // bufferAllocate
    this->componentIn.set_dataOutAllocate_OutputPort(0, this->get_from_dataOutAllocate(0));

    // dataDeallocate
    this->componentOut.set_dataInDeallocate_OutputPort(0, this->get_from_dataInDeallocate(0));

    // bufferDeallocate
    this->componentIn.set_bufferDeallocate_OutputPort(0, this->get_from_bufferDeallocate(0));

    // ----------------------------------------------------------------------
    // Connect serial output ports
    // ----------------------------------------------------------------------
    max = std::min(this->componentIn.getNum_portIn_InputPorts(), this->componentOut.getNum_portOut_OutputPorts());
    for (U32 i = 0; i < max; ++i) {
        this->componentOut.set_portOut_OutputPort(i, this->get_from_portOut(i));
    }

    // ----------------------------------------------------------------------
    // Connect serial input ports
    // ----------------------------------------------------------------------
    // portIn
    for (U32 i = 0; i < max; ++i) {
        this->connect_to_portIn(i, this->componentIn.get_portIn_InputPort(i));
    }
}

void GenericHubTester ::initComponents() {
    this->init();
    this->componentIn.init(INSTANCE);
    this->componentOut.init(INSTANCE + 1);
}

}  // end namespace Svc
