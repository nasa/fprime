// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  cpp file for ComQueue test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include "Fw/Types/BasicTypes.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester() : ComQueueGTestBase("Tester", MAX_HISTORY_SIZE), component("ComQueue") {
    this->component.configure(QueueConfiguration queueConfig[], NATIVE_UINT_TYPE configSize, Fw::MemAllocator allocator);
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testThrottle() {
    // TODO
}

void Tester ::testRoundRobin() {
    // TODO
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_buffQueueSend_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_buffQueueSend(fwBuffer);
}

void Tester ::from_comQueueSend_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comQueueSend(data, context);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // buffQueueIn
    this->connect_to_buffQueueIn(0, this->component.get_buffQueueIn_InputPort(0));

    // comQueueIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_comQueueIn(i, this->component.get_comQueueIn_InputPort(i));
    }

    // comStatusIn
    this->connect_to_comStatusIn(0, this->component.get_comStatusIn_InputPort(0));

    // run
    this->connect_to_run(0, this->component.get_run_InputPort(0));

    // Log
    this->component.set_Log_OutputPort(0, this->get_from_Log(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    // Time
    this->component.set_Time_OutputPort(0, this->get_from_Time(0));

    // Tlm
    this->component.set_Tlm_OutputPort(0, this->get_from_Tlm(0));

    // buffQueueSend
    this->component.set_buffQueueSend_OutputPort(0, this->get_from_buffQueueSend(0));

    // comQueueSend
    this->component.set_comQueueSend_OutputPort(0, this->get_from_comQueueSend(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
}

}  // end namespace Svc
