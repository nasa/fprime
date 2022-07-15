// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  cpp file for ComQueue test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include "Fw/Types/MallocAllocator.hpp"
using namespace std;

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100
#define QUEUE_DEPTH 100

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester() : ComQueueGTestBase("Tester", MAX_HISTORY_SIZE), component("ComQueue") {
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testQueueSend() {
    U8 data[BUFFER_LENGTH] = {0xde,0xad,0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));

    QueueConfiguration queue[totalSize];
    Fw::MallocAllocator allocator;
    Svc::ComSendStatus state = Svc::ComSendStatus::READY;

    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++){
        queue[i].priority = i;
        queue[i].depth = 3;
    }
    component.configure(queue, totalSize, allocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_comQueueIn_InputPorts(); portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        invoke_to_comStatusIn(0, state);
        component.doDispatch();
        ASSERT_from_comQueueSend(0, comBuffer, 0);
        clearFromPortHistory();
    }

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_buffQueueIn_InputPorts(); portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        invoke_to_comStatusIn(0, state);
        component.doDispatch();
        ASSERT_from_buffQueueSend(0, buffer);
        clearFromPortHistory();
    }
}

void Tester ::testRetrySend(){
    U8 data[BUFFER_LENGTH] = {'a','b','c'};
    U8 dataGarbage[BUFFER_LENGTH] = {'c', 'd', 'e'};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::ComBuffer comBufferGarbage(&dataGarbage[0], sizeof(dataGarbage));

    Fw::Buffer buffer(&data[0], sizeof(data));
    Fw::Buffer bufferGarbage(&dataGarbage[0], sizeof(dataGarbage));

    QueueConfiguration queue[totalSize];
    Fw::MallocAllocator allocator;
    Svc::ComSendStatus readyState = Svc::ComSendStatus::READY;
    Svc::ComSendStatus failState = Svc::ComSendStatus::FAIL;

    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++){
        queue[i].priority = i;
        queue[i].depth = 3;
    }
    component.configure(queue, totalSize, allocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_comQueueIn_InputPorts(); portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();

        invoke_to_comStatusIn(0, failState);
        component.doDispatch();

        invoke_to_comQueueIn(portNum,  comBufferGarbage, 0);
        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();

        ASSERT_from_comQueueSend_SIZE(2);
        ASSERT_from_comQueueSend(1, comBuffer, 0);
        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();

        ASSERT_from_comQueueSend(0, comBuffer, 0);
        clearFromPortHistory();
    }

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_buffQueueIn_InputPorts(); portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();

        invoke_to_comStatusIn(0,failState);
        component.doDispatch();
        // verify command was sent

        invoke_to_buffQueueIn(portNum, bufferGarbage);
        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();
        ASSERT_from_buffQueueSend_SIZE(2);

        invoke_to_comStatusIn(0, readyState);
        component.doDispatch();
        ASSERT_from_buffQueueSend_SIZE(3);
        ASSERT_from_buffQueueSend(1, buffer);

        clearFromPortHistory();
    }
}

void Tester ::testPrioritySend(){
    U8 data[totalSize][BUFFER_LENGTH];

    QueueConfiguration queue[totalSize];
    Fw::MallocAllocator allocator;
    Svc::ComSendStatus state = Svc::ComSendStatus::READY;

    U8 priorityList[totalSize];
    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++){
        priorityList[i] = i;
    }
    priorityList[totalSize - 1] = 0;

    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++){
        queue[i].priority = totalSize - i - 1;
        queue[i].depth = 3;
        data[i][0] = totalSize - i - 1;
    }

    // Make the last message have the same priority as the second message
    queue[totalSize - 1].priority = 1;
    data[totalSize - 2][0] = 0;
    data[totalSize - 1][0] = 1;

    component.configure(queue, totalSize, allocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_comQueueIn_InputPorts(); portNum++){
        Fw::ComBuffer comBuffer(&data[portNum][0], BUFFER_LENGTH);
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        invoke_to_comStatusIn(0, state);
    }


    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_buffQueueIn_InputPorts(); portNum++){
        Fw::Buffer buffer(&data[portNum + component.getNum_comQueueIn_InputPorts()][0], BUFFER_LENGTH);
        invoke_to_buffQueueIn(portNum, buffer);
        invoke_to_comStatusIn(0, state);
    }

    for (NATIVE_INT_TYPE index = 0; index < totalSize; index++){
        U8 orderKey;
        component.doDispatch();
        ASSERT_EQ(fromPortHistory_comQueueSend->size() + fromPortHistory_buffQueueSend->size(), 1);
        if (fromPortHistory_comQueueSend->size() == 1){
            orderKey = fromPortHistory_comQueueSend->at(0).data.getBuffAddr()[0];
        } else {
            orderKey = fromPortHistory_buffQueueSend->at(0).fwBuffer.getData()[0];
        }
        ASSERT_EQ(orderKey, index);
        clearFromPortHistory();
    }

}

void Tester::testQueueFull(){
    U8 data[] = {'a', 'b', 'c', 'd'};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));


    QueueConfiguration queue[totalSize];
    Fw::MallocAllocator allocator;
    Svc::ComSendStatus state = Svc::ComSendStatus::READY;

    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++){
        queue[i].priority = i;
        queue[i].depth = 2;
    }

    component.configure(queue, totalSize, allocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < component.getNum_comQueueIn_InputPorts(); portNum++){
        // queue[portNum].depth + 1 to deliberately cause overflow
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < queue[portNum].depth + 1; msgCount++) {
            invoke_to_comQueueIn(portNum, comBuffer, 0);
        }
        ASSERT_EVENTS_QueueFull(portNum, QueueType::comQueue, portNum);
    }

    for(NATIVE_INT_TYPE portNum = component.getNum_comQueueIn_InputPorts(); portNum < component.getNum_buffQueueIn_InputPorts(); portNum++){
        // queue[portNum].depth + 1 to deliberately cause overflow
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < queue[portNum].depth + 1; msgCount++) {
            invoke_to_buffQueueIn(portNum, buffer);
        }
        ASSERT_EVENTS_QueueFull(portNum, QueueType::buffQueue, portNum);
    }

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
