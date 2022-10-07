// ======================================================================
// \title  ComStub.hpp
// \author mstarch
// \brief  cpp file for ComStub test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include <STest/Pick/Pick.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100
#define RETRIES 3

U8 storage[RETRIES][10240];

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : ComStubGTestBase("Tester", MAX_HISTORY_SIZE),
      m_component("ComStub"),
      m_send_mode(Drv::SendStatus::SEND_OK),
      m_retries(0) {
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------
void Tester ::fill(Fw::Buffer& buffer_to_fill) {
    U8 size = STest::Pick::lowerUpper(1, sizeof(buffer_to_fill.getSize()));
    for (U32 i = 0; i < size; i++) {
        buffer_to_fill.getData()[i] = STest::Pick::any();
    }
    buffer_to_fill.setSize(size);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void Tester ::test_initial() {
    Fw::Success condition = Fw::Success::SUCCESS;
    invoke_to_drvConnected(0);
    ASSERT_from_comStatus_SIZE(1);
    ASSERT_from_comStatus(0, condition);
    this->fromPortHistory_comStatus->clear();
}

void Tester ::test_basic() {
    this->test_initial();
    Fw::Buffer buffer(storage[0], sizeof(storage[0]));
    Fw::Success condition = Fw::Success::SUCCESS;
    this->fill(buffer);

    // Downlink
    ASSERT_EQ(invoke_to_comDataIn(0, buffer), Drv::SendStatus::SEND_OK);
    ASSERT_from_drvDataOut_SIZE(1);
    ASSERT_from_drvDataOut(0, buffer);
    ASSERT_from_comStatus(0, condition);

    // Uplink
    Drv::RecvStatus status = Drv::RecvStatus::RECV_OK;
    invoke_to_drvDataIn(0, buffer, status);
    ASSERT_from_comDataOut_SIZE(1);
    ASSERT_from_comDataOut(0, buffer, status);
}

void Tester ::test_fail() {
    this->test_initial();
    Fw::Buffer buffer(storage[0], sizeof(storage[0]));
    this->fill(buffer);
    Fw::Success condition = Fw::Success::FAILURE;
    m_send_mode = Drv::SendStatus::SEND_ERROR;

    // Downlink
    ASSERT_EQ(invoke_to_comDataIn(0, buffer), Drv::SendStatus::SEND_OK);
    ASSERT_from_drvDataOut_SIZE(1);
    ASSERT_from_drvDataOut(0, buffer);
    ASSERT_from_drvDataOut_SIZE(1);
    ASSERT_from_comStatus(0, condition);

    // Uplink
    Drv::RecvStatus status = Drv::RecvStatus::RECV_ERROR;
    invoke_to_drvDataIn(0, buffer, status);
    ASSERT_from_comDataOut_SIZE(1);
    ASSERT_from_comDataOut(0, buffer, status);
}

void Tester ::test_retry() {
    this->test_initial();
    Fw::Buffer buffers[RETRIES];
    Fw::Success condition = Fw::Success::SUCCESS;
    m_send_mode = Drv::SendStatus::SEND_RETRY;

    for (U32 i = 0; i < RETRIES; i++) {
        buffers[i].setData(storage[i]);
        buffers[i].setSize(sizeof(storage[i]));
        buffers[i].setContext(i);
        this->fill(buffers[i]);
        invoke_to_comDataIn(0, buffers[i]);
        ASSERT_from_drvDataOut_SIZE((i + 1) * RETRIES);
        m_retries = 0;
    }
    ASSERT_from_drvDataOut_SIZE(RETRIES * RETRIES);
    ASSERT_from_comStatus_SIZE(3);
    for (U32 i = 0; i < RETRIES; i++) {
        for (U32 j = 0; j < RETRIES; j++) {
            ASSERT_from_drvDataOut((i * RETRIES) + j, buffers[i]);
        }
        ASSERT_from_comStatus(i, condition);
    }
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_comDataOut_handler(const NATIVE_INT_TYPE portNum,
                                      Fw::Buffer& recvBuffer,
                                      const Drv::RecvStatus& recvStatus) {
    this->pushFromPortEntry_comDataOut(recvBuffer, recvStatus);
}

void Tester ::from_comStatus_handler(const NATIVE_INT_TYPE portNum, Fw::Success& condition) {
    this->pushFromPortEntry_comStatus(condition);
}

Drv::SendStatus Tester ::from_drvDataOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    this->pushFromPortEntry_drvDataOut(sendBuffer);
    m_retries = (m_send_mode == Drv::SendStatus::SEND_RETRY) ? (m_retries + 1) : m_retries;
    if (m_retries < RETRIES) {
        return m_send_mode;
    }
    return Drv::SendStatus::SEND_OK;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // comDataIn
    this->connect_to_comDataIn(0, this->m_component.get_comDataIn_InputPort(0));

    // drvConnected
    this->connect_to_drvConnected(0, this->m_component.get_drvConnected_InputPort(0));

    // drvDataIn
    this->connect_to_drvDataIn(0, this->m_component.get_drvDataIn_InputPort(0));

    // comDataOut
    this->m_component.set_comDataOut_OutputPort(0, this->get_from_comDataOut(0));

    // comStatus
    this->m_component.set_comStatus_OutputPort(0, this->get_from_comStatus(0));

    // drvDataOut
    this->m_component.set_drvDataOut_OutputPort(0, this->get_from_drvDataOut(0));
}

void Tester ::initComponents() {
    this->init();
    this->m_component.init(INSTANCE);
}

}  // end namespace Svc
