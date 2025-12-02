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
    U32 max =
        std::min(this->componentIn.getNum_serialIn_InputPorts(), this->componentOut.getNum_serialOut_OutputPorts());
    for (U32 i = 0; i < max; i++) {
        send_random_comm(i);
        ASSERT_from_fromBufferDriverReturn_SIZE(1);
        fromPortHistory_fromBufferDriverReturn->clear();
    }
}

void GenericHubTester ::test_buffer_io() {
    U32 max =
        std::min(this->componentIn.getNum_bufferIn_InputPorts(), this->componentOut.getNum_bufferOut_OutputPorts());
    for (U32 i = 0; i < max; i++) {
        send_random_buffer(i);
        ASSERT_from_fromBufferDriverReturn_SIZE(1);
        fromPortHistory_fromBufferDriverReturn->clear();
    }
}

void GenericHubTester ::test_random_io() {
    for (U32 i = 0; i < 10000; i++) {
        U32 choice = STest::Pick::lowerUpper(0, 1);
        if (choice) {
            U32 port = STest::Pick::lowerUpper(0, std::min(this->componentIn.getNum_serialIn_InputPorts(),
                                                           this->componentOut.getNum_serialOut_OutputPorts()) -
                                                      1);
            send_random_comm(port);
        } else {
            U32 port = STest::Pick::lowerUpper(0, std::min(this->componentIn.getNum_bufferIn_InputPorts(),
                                                           this->componentOut.getNum_bufferOut_OutputPorts()) -
                                                      1);
            send_random_buffer(port);
        }
        ASSERT_from_fromBufferDriverReturn_SIZE(1);
        fromPortHistory_fromBufferDriverReturn->clear();
    }
}

void GenericHubTester ::random_fill(Fw::SerializeBufferBase& buffer, U32 max_size) {
    U32 random_size = STest::Pick::lowerUpper(0, max_size);
    buffer.resetSer();
    for (U32 i = 0; i < random_size; i++) {
        buffer.serializeFrom(static_cast<U8>(STest::Pick::any()));
    }
}

void GenericHubTester ::test_telemetry() {
    Fw::TlmBuffer buffer;
    clearFromPortHistory();
    random_fill(buffer, FW_TLM_BUFFER_MAX_SIZE);

    Fw::Time time(100, 200);
    invoke_to_tlmIn(0, 123, time, buffer);

    // **must** return buffer
    ASSERT_from_fromBufferDriverReturn_SIZE(1);
    ASSERT_from_tlmOut_SIZE(1);
    ASSERT_from_tlmOut(0, 123, time, buffer);
    clearFromPortHistory();
}

void GenericHubTester ::test_events() {
    Fw::LogSeverity severity = Fw::LogSeverity::WARNING_HI;
    Fw::LogBuffer buffer;
    random_fill(buffer, FW_LOG_BUFFER_MAX_SIZE);

    Fw::Time time(100, 200);
    invoke_to_eventIn(0, 123, time, severity, buffer);

    // **must** deallocate buffer
    ASSERT_from_fromBufferDriverReturn_SIZE(1);
    ASSERT_from_eventOut_SIZE(1);
    ASSERT_from_eventOut(0, 123, time, severity, buffer);
    clearFromPortHistory();
}
// Helpers

void GenericHubTester ::send_random_comm(U32 port) {
    random_fill(m_comm, FW_COM_BUFFER_MAX_SIZE);
    m_current_port = port;
    invoke_to_serialIn(m_current_port, m_comm);
    // Ensure that the data out was called, and that the serialOut unwrapped properly
    ASSERT_from_toBufferDriver_SIZE(m_comm_in + m_buffer_out + 1);
    ASSERT_EQ(m_comm_in + 1, m_comm_out);
    m_comm_in++;
}

void GenericHubTester ::send_random_buffer(U32 port) {
    U32 max_random_size = STest::Pick::lowerUpper(0, DATA_SIZE - (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    m_buffer.set(m_data_store, sizeof(m_data_store));
    ASSERT_GE(m_buffer.getSize(), max_random_size);
    auto serializer = m_buffer.getSerializer();
    random_fill(serializer, max_random_size);
    m_buffer.setSize(max_random_size);
    m_current_port = port;
    invoke_to_bufferIn(m_current_port, m_buffer);
    ASSERT_from_bufferInReturn_SIZE(1);
    ASSERT_from_bufferInReturn(0, m_buffer);
    fromPortHistory_bufferInReturn->clear();
    // Ensure that the data out was called, and that the serialOut unwrapped properly
    ASSERT_from_toBufferDriver_SIZE(m_buffer_in + m_comm_out + 1);
    ASSERT_EQ(m_buffer_in + 1, m_buffer_out);
    m_buffer_in++;
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void GenericHubTester ::from_eventOut_handler(const FwIndexType portNum,
                                              FwEventIdType id,
                                              Fw::Time& timeTag,
                                              const Fw::LogSeverity& severity,
                                              Fw::LogBuffer& args) {
    this->pushFromPortEntry_eventOut(id, timeTag, severity, args);
}

void GenericHubTester ::from_tlmOut_handler(const FwIndexType portNum,
                                            FwChanIdType id,
                                            Fw::Time& timeTag,
                                            Fw::TlmBuffer& val) {
    this->pushFromPortEntry_tlmOut(id, timeTag, val);
}

void GenericHubTester ::from_toBufferDriver_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    ASSERT_NE(fwBuffer.getData(), nullptr) << "Empty buffer to deallocate";
    ASSERT_GE(fwBuffer.getData(), m_data_for_allocation) << "Incorrect data pointer deallocated";
    ASSERT_LT(fwBuffer.getData(), m_data_for_allocation + sizeof(m_data_for_allocation))
        << "Incorrect data pointer deallocated";
    // Reuse m_allocate to pass into the otherside of the hub
    this->pushFromPortEntry_toBufferDriver(fwBuffer);
    invoke_to_fromBufferDriver(0, fwBuffer);
}

// ----------------------------------------------------------------------
// Handlers for serial from ports
// ----------------------------------------------------------------------

void GenericHubTester ::from_bufferOut_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    m_buffer_out++;
    // Assert the buffer came through exactly on the right port
    ASSERT_EQ(portNum, m_current_port);
    ASSERT_EQ(fwBuffer.getSize(), m_buffer.getSize());
    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        U8 byte1 = reinterpret_cast<U8*>(fwBuffer.getData())[i];
        U8 byte2 = reinterpret_cast<U8*>(m_buffer.getData())[i];
        ASSERT_EQ(byte1, byte2);
    }

    this->invoke_to_bufferOutReturn(portNum, fwBuffer);
    // this->from_fromBufferDriverReturn_handler(0, fwBuffer);
}

void GenericHubTester ::from_serialOut_handler(FwIndexType portNum,            /*!< The port number*/
                                               Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    m_comm_out++;
    // Assert the buffer came through exactly on the right port
    ASSERT_EQ(portNum, m_current_port);
    ASSERT_EQ(Buffer.getSize(), m_comm.getSize());
    for (U32 i = 0; i < Buffer.getSize(); i++) {
        ASSERT_EQ(Buffer.getBuffAddr()[i], m_comm.getBuffAddr()[i]);
    }
    ASSERT_from_bufferOut_SIZE(0);
}

Fw::Buffer GenericHubTester ::from_allocate_handler(const FwIndexType portNum, const FwSizeType size) {
    EXPECT_EQ(m_allocate.getData(), nullptr) << "Allocation buffer is still in use";
    EXPECT_LE(size, sizeof(m_data_for_allocation)) << "Allocation buffer size mismatch";
    m_allocate.set(m_data_for_allocation, size);
    return m_allocate;
}

void GenericHubTester ::from_bufferInReturn_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Check buffer deallocations here
    ASSERT_EQ(fwBuffer.getData(), m_buffer.getData()) << "Ensure that the buffer was deallocated";
    ASSERT_EQ(fwBuffer.getSize(), m_buffer.getSize()) << "Ensure that the buffer was deallocated";
    this->pushFromPortEntry_bufferInReturn(fwBuffer);
}

void GenericHubTester ::from_fromBufferDriverReturn_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    ASSERT_NE(fwBuffer.getData(), nullptr) << "Empty buffer to deallocate";
    ASSERT_GE(fwBuffer.getData(), m_data_for_allocation) << "Incorrect data pointer deallocated";
    ASSERT_LT(fwBuffer.getData(), m_data_for_allocation + sizeof(m_data_for_allocation))
        << "Incorrect data pointer deallocated";

    this->pushFromPortEntry_fromBufferDriverReturn(fwBuffer);
    invoke_to_toBufferDriverReturn(portNum, fwBuffer);
    m_allocate.set(nullptr, 0);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void GenericHubTester ::connectPorts() {
    // bufferIn
    U32 max =
        std::min(this->componentIn.getNum_bufferIn_InputPorts(), this->componentOut.getNum_bufferOut_OutputPorts());
    for (U32 i = 0; i < max; ++i) {
        this->connect_to_bufferIn(i, this->componentIn.get_bufferIn_InputPort(i));
    }

    // eventIn
    this->connect_to_eventIn(0, this->componentIn.get_eventIn_InputPort(0));

    // tlmIn
    this->connect_to_tlmIn(0, this->componentIn.get_tlmIn_InputPort(0));

    // fromBufferDriver
    this->connect_to_fromBufferDriver(0, this->componentOut.get_fromBufferDriver_InputPort(0));

    // bufferOut
    for (U32 i = 0; i < max; ++i) {
        this->componentOut.set_bufferOut_OutputPort(i, this->get_from_bufferOut(i));
    }

    // eventOut
    this->componentOut.set_eventOut_OutputPort(0, this->get_from_eventOut(0));

    // tlmOut
    this->componentOut.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // toBufferDriver
    this->componentIn.set_toBufferDriver_OutputPort(0, this->get_from_toBufferDriver(0));

    // toBufferDriverReturn
    this->connect_to_toBufferDriverReturn(0, this->componentIn.get_toBufferDriverReturn_InputPort(0));

    // bufferOutReturn
    for (U32 i = 0; i < max; ++i) {
        this->connect_to_bufferOutReturn(i, this->componentOut.get_bufferOutReturn_InputPort(i));
    }

    // bufferAllocate
    this->componentIn.set_allocate_OutputPort(0, this->get_from_allocate(0));

    // buffer Return
    this->componentOut.set_fromBufferDriverReturn_OutputPort(0, this->get_from_fromBufferDriverReturn(0));

    // dataDeallocate
    this->componentIn.set_deallocate_OutputPort(0, this->get_from_deallocate(0));

    // bufferInReturn
    for (FwIndexType i = 0; i < GenericHubCfg::NumBufferInputPorts; i++) {
        this->componentIn.set_bufferInReturn_OutputPort(i, this->get_from_bufferInReturn(i));
    }

    // ----------------------------------------------------------------------
    // Connect serial output ports
    // ----------------------------------------------------------------------
    max = std::min(this->componentIn.getNum_serialIn_InputPorts(), this->componentOut.getNum_serialOut_OutputPorts());
    for (U32 i = 0; i < max; ++i) {
        this->componentOut.set_serialOut_OutputPort(i, this->get_from_serialOut(i));
    }

    // ----------------------------------------------------------------------
    // Connect serial input ports
    // ----------------------------------------------------------------------
    // serialIn
    for (U32 i = 0; i < max; ++i) {
        this->connect_to_serialIn(i, this->componentIn.get_serialIn_InputPort(i));
    }
}

void GenericHubTester ::initComponents() {
    this->init();
    this->componentIn.init(INSTANCE);
    this->componentOut.init(INSTANCE + 1);
}

}  // end namespace Svc
