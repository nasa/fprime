// ======================================================================
// \title  BufferRepeater.hpp
// \author lestarch
// \brief  cpp file for BufferRepeater test harness implementation class
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : BufferRepeaterGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferRepeater"),
      m_port_index_history(MAX_HISTORY_SIZE),
      m_initial_buffer(),
      m_failure(false) {
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testRepeater() {
    this->component.configure(BufferRepeater::FATAL_ON_OUT_OF_MEMORY);
    m_initial_buffer.setSize(1024);
    m_initial_buffer.setData(new U8[1024]);
    for (U32 i = 0; i < m_initial_buffer.getSize(); i++) {
        m_initial_buffer.getData()[i] = static_cast<U8>(i);
    }

    invoke_to_portIn(0, m_initial_buffer);
    ASSERT_EVENTS_AllocationHardFailure_SIZE(0);
    ASSERT_EVENTS_AllocationSoftFailure_SIZE(0);
    ASSERT_from_portOut_SIZE(this->component.getNum_portOut_OutputPorts());
    ASSERT_EQ(fromPortHistory_portOut->size(), this->m_port_index_history.size());
    for (NATIVE_INT_TYPE i = 0; i < this->component.getNum_portOut_OutputPorts(); i++) {
        NATIVE_INT_TYPE port_index = this->m_port_index_history.at(i);
        ASSERT_EQ(i, port_index);
        Fw::Buffer buffer_under_test = this->fromPortHistory_portOut->at(i).fwBuffer;
        ASSERT_EQ(buffer_under_test.getSize(), m_initial_buffer.getSize());
        for (U32 j = 0; j < FW_MIN(buffer_under_test.getSize(), m_initial_buffer.getSize()); j++) {
            ASSERT_EQ(buffer_under_test.getData()[j], m_initial_buffer.getData()[j])
                << "Data not copied correctly at offset: " << j;
        }
        // Deallocate allocated data
        if (buffer_under_test.getData() != nullptr) {
            delete[] buffer_under_test.getData();
        }
    }
    // Check proper deallocation
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(m_initial_buffer.getData(), fromPortHistory_deallocate->at(0).fwBuffer.getData());
    ASSERT_EQ(m_initial_buffer.getSize(), fromPortHistory_deallocate->at(0).fwBuffer.getSize());
    delete[] m_initial_buffer.getData();
    m_initial_buffer.setData(nullptr);
}

void Tester ::testFailure(BufferRepeater::BufferRepeaterFailureOption failure_option) {
    this->m_failure = true;
    this->component.configure(failure_option);
    m_initial_buffer.setSize(1024);
    m_initial_buffer.setData(new U8[1024]);

    invoke_to_portIn(0, m_initial_buffer);
    switch (failure_option) {
        case BufferRepeater::WARNING_ON_OUT_OF_MEMORY:
            ASSERT_EVENTS_AllocationHardFailure_SIZE(0);
            ASSERT_EVENTS_AllocationSoftFailure_SIZE(this->component.getNum_portOut_OutputPorts());
            break;
        case BufferRepeater::FATAL_ON_OUT_OF_MEMORY:
            ASSERT_EVENTS_AllocationHardFailure_SIZE(this->component.getNum_portOut_OutputPorts());
            ASSERT_EVENTS_AllocationSoftFailure_SIZE(0);
            break;
        // Cascade intended
        case BufferRepeater::NO_RESPONSE_ON_OUT_OF_MEMORY:
        case BufferRepeater::NUM_BUFFER_REPEATER_FAILURE_OPTIONS:
            ASSERT_EVENTS_AllocationHardFailure_SIZE(0);
            ASSERT_EVENTS_AllocationSoftFailure_SIZE(0);
            break;
    }

    ASSERT_from_portOut_SIZE(0);

    // Check proper deallocation
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(m_initial_buffer.getData(), fromPortHistory_deallocate->at(0).fwBuffer.getData());
    ASSERT_EQ(m_initial_buffer.getSize(), fromPortHistory_deallocate->at(0).fwBuffer.getSize());
    delete[] m_initial_buffer.getData();
    m_initial_buffer.setData(nullptr);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer Tester ::from_allocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_allocate(size);
    Fw::Buffer new_buffer;

    if (m_failure) {
        new_buffer.setSize(0);
        new_buffer.setData(nullptr);
    } else {
        new_buffer.setSize(size);
        new_buffer.setData(new U8[size]);
    }
    return new_buffer;
}

void Tester ::from_deallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_deallocate(fwBuffer);
    EXPECT_EQ(fwBuffer.getData(), m_initial_buffer.getData()) << "Deallocated non-initial buffer";
}

void Tester ::from_portOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->m_port_index_history.push_back(portNum);
    this->pushFromPortEntry_portOut(fwBuffer);
    EXPECT_NE(fwBuffer.getData(), nullptr) << "Passed invalid buffer out port";
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // portIn
    this->connect_to_portIn(0, this->component.get_portIn_InputPort(0));

    // Log
    this->component.set_Log_OutputPort(0, this->get_from_Log(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    // Time
    this->component.set_Time_OutputPort(0, this->get_from_Time(0));

    // allocate
    this->component.set_allocate_OutputPort(0, this->get_from_allocate(0));

    // deallocate
    this->component.set_deallocate_OutputPort(0, this->get_from_deallocate(0));

    // portOut
    for (NATIVE_INT_TYPE i = 0; i < this->component.getNum_portOut_OutputPorts(); ++i) {
        this->component.set_portOut_OutputPort(i, this->get_from_portOut(i));
    }
}

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
