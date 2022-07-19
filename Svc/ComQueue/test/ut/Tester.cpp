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

void Tester ::configure() {
    QueueConfiguration queue[ComQueue::totalSize];
    Fw::MallocAllocator allocator;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::totalSize; i++){
        queue[i].priority = i;
        queue[i].depth = 3;
    }
    component.configure(queue, ComQueue::totalSize, allocator);
}

void Tester ::sendByQueueNumber(NATIVE_INT_TYPE queueNum, NATIVE_INT_TYPE& portNum, QueueType& queueType) {
    U8 data[BUFFER_LENGTH] = {0xde,0xad,0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    if (queueNum < ComQueueComSize) {
        portNum = queueNum;
        queueType = QueueType::comQueue;
        invoke_to_comQueueIn(portNum, comBuffer, 0);
    } else {
        portNum = queueNum - ComQueueComSize;
        queueType = QueueType::buffQueue;
        invoke_to_buffQueueIn(portNum, buffer);
    }
}

void Tester ::emitOne() {
    Svc::ComSendStatus state = Svc::ComSendStatus::READY;
    invoke_to_comStatusIn(0, state);
    component.doDispatch();
}

void Tester ::emitOneAndCheck(NATIVE_UINT_TYPE expectedIndex, QueueType expectedType, Fw::ComBuffer& expectedCom, Fw::Buffer& expectedBuff) {
    emitOne();

    if (expectedType == QueueType::comQueue) {
        ASSERT_from_comQueueSend(expectedIndex, expectedCom, 0);
    } else {
        ASSERT_from_buffQueueSend(expectedIndex, expectedBuff);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testQueueSend() {
    U8 data[BUFFER_LENGTH] = {0xde,0xad,0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueComSize; portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        emitOneAndCheck(0, QueueType::comQueue, comBuffer, buffer);
        clearFromPortHistory();
    }

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueBuffSize; portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        emitOneAndCheck(0, QueueType::buffQueue, comBuffer, buffer);
        clearFromPortHistory();
    }
}

void Tester ::testRetrySend() {
    U8 data[BUFFER_LENGTH] = {'a','b','c'};
    U8 dataGarbage[BUFFER_LENGTH] = {'c', 'd', 'e'};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::ComBuffer comBufferGarbage(&dataGarbage[0], sizeof(dataGarbage));

    Fw::Buffer buffer(&data[0], sizeof(data));
    Fw::Buffer bufferGarbage(&dataGarbage[0], sizeof(dataGarbage));


    Svc::ComSendStatus readyState = Svc::ComSendStatus::READY;
    Svc::ComSendStatus failState = Svc::ComSendStatus::FAIL;

    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueComSize; portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        invoke_to_comQueueIn(portNum,  comBufferGarbage, 0); // Send in garbage to ensure the right retry
        emitOneAndCheck(0, QueueType::comQueue, comBuffer, buffer);

        // Fail and force retry
        invoke_to_comStatusIn(0, failState);
        component.doDispatch();

        // Retry should be original buffer
        emitOneAndCheck(1, QueueType::comQueue, comBuffer, buffer);

        ASSERT_from_comQueueSend_SIZE(2);
        // Now clear out the garbage in queue
        emitOneAndCheck(2, QueueType::comQueue, comBufferGarbage, bufferGarbage);
        clearFromPortHistory();
    }

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueBuffSize; portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        invoke_to_buffQueueIn(portNum, bufferGarbage); // Send in garbage to ensure the right retry
        emitOneAndCheck(0, QueueType::buffQueue, comBuffer, buffer);

        // Fail and force retry
        invoke_to_comStatusIn(0,failState);
        component.doDispatch();

        // Retry should be original buffer
        emitOneAndCheck(1, QueueType::buffQueue, comBuffer, buffer);
        ASSERT_from_buffQueueSend_SIZE(2);

        // Now clear out the garbage in queue
        emitOneAndCheck(2, QueueType::buffQueue, comBufferGarbage, bufferGarbage);
        clearFromPortHistory();
    }
}

void Tester ::testPrioritySend(){
    U8 data[ComQueue::totalSize][BUFFER_LENGTH];

    QueueConfiguration queue[ComQueue::totalSize];
    Fw::MallocAllocator allocator;

    U8 priorityList[ComQueue::totalSize];
    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::totalSize; i++){
        priorityList[i] = i;
    }
    priorityList[ComQueue::totalSize - 1] = 0;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::totalSize; i++){
        queue[i].priority = ComQueue::totalSize - i - 1;
        queue[i].depth = 3;
        data[i][0] = ComQueue::totalSize - i - 1;
    }

    // Make the last message have the same priority as the second message
    queue[ComQueue::totalSize - 1].priority = 1;
    data[ComQueue::totalSize - 2][0] = 0;
    data[ComQueue::totalSize - 1][0] = 1;

    component.configure(queue, ComQueue::totalSize, allocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueComSize; portNum++){
        Fw::ComBuffer comBuffer(&data[portNum][0], BUFFER_LENGTH);
        invoke_to_comQueueIn(portNum, comBuffer, 0);
    }


    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueBuffSize; portNum++){
        Fw::Buffer buffer(&data[portNum + ComQueueComSize][0], BUFFER_LENGTH);
        invoke_to_buffQueueIn(portNum, buffer);
    }

    for (NATIVE_INT_TYPE index = 0; index < ComQueue::totalSize; index++){
        U8 orderKey;
        emitOne();
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
    QueueConfiguration queue[ComQueue::totalSize];
    Fw::MallocAllocator allocator;
    ComQueueDepth expectedComDepth;
    BuffQueueDepth expectedBuffDepth;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::totalSize; i++){
        queue[i].priority = i;
        queue[i].depth = 2;

        // Expected depths
        if (i < ComQueueComSize) {
            expectedComDepth[i] = queue[i].depth;
        } else {
            expectedBuffDepth[i - ComQueueComSize] = queue[i].depth;
        }

    }

    component.configure(queue,ComQueue::totalSize, allocator);

    for(NATIVE_INT_TYPE queueNum = 0; queueNum < ComQueue::totalSize; queueNum++) {
        QueueType overflow_type;
        NATIVE_INT_TYPE portNum;
        // queue[portNum].depth + 2 to deliberately cause overflow and check throttle of exactly 1
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < queue[queueNum].depth + 2; msgCount++) {
            sendByQueueNumber(queueNum, portNum, overflow_type);
        }
        ASSERT_EVENTS_QueueFull_SIZE(1);
        ASSERT_EVENTS_QueueFull(0, overflow_type, portNum);

        // Drain a message, and see if throttle resets
        emitOne();

        // Force another overflow by filling then deliberately overflowing the queue
        sendByQueueNumber(queueNum, portNum, overflow_type);
        sendByQueueNumber(queueNum, portNum, overflow_type);

        ASSERT_EVENTS_QueueFull_SIZE(2);
        ASSERT_EVENTS_QueueFull(1, overflow_type, portNum);

        // Drain the queue again such that we have a clean slate before the next queue
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < queue[queueNum].depth; msgCount++) {
            emitOne();
        }
        clearEvents();
    }
    // Check max seen queue-depths
    invoke_to_run(0, 0);
    component.doDispatch();
    ASSERT_TLM_comQueueDepth_SIZE(1);
    ASSERT_TLM_buffQueueDepth_SIZE(1);
    ASSERT_TLM_comQueueDepth(0, expectedComDepth);
    ASSERT_TLM_buffQueueDepth(0, expectedBuffDepth);
}

void Tester ::testReadyFirst() {
    U8 data[BUFFER_LENGTH] = {0xde,0xad,0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueComSize; portNum++){
        emitOne();
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        ASSERT_from_comQueueSend(0, comBuffer, 0);
        clearFromPortHistory();
    }

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueueBuffSize; portNum++){
        emitOne();
        invoke_to_buffQueueIn(portNum, buffer);
        ASSERT_from_buffQueueSend(0, buffer);
        clearFromPortHistory();
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
