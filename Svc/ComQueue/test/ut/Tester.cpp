// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  cpp file for ComQueue test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include "Fw/Types/MallocAllocator.hpp"
using namespace std;

Fw::MallocAllocator mallocAllocator;
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

void Tester ::dispatchAll() {
    while (this->component.m_queue.getNumMsgs() > 0) {
        this->component.doDispatch();
    }
}

void Tester ::configure() {
    ComQueue::QueueConfigurationTable configurationTable;
    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++){
        configurationTable.entries[i].priority = i;
        configurationTable.entries[i].depth = 3;
    }
    component.configure(configurationTable, 0, mallocAllocator);
}

void Tester ::sendByQueueNumber(NATIVE_INT_TYPE queueNum, NATIVE_INT_TYPE& portNum, QueueType& queueType) {
    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    if (queueNum < ComQueue::COM_PORT_COUNT) {
        portNum = queueNum;
        queueType = QueueType::COM_QUEUE;
        invoke_to_comQueueIn(portNum, comBuffer, 0);
    } else {
        portNum = queueNum - ComQueue::COM_PORT_COUNT;
        queueType = QueueType::BUFFER_QUEUE;
        invoke_to_buffQueueIn(portNum, buffer);
    }
}

void Tester ::emitOne() {
    Fw::Success state = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, state);
    dispatchAll();
}

void Tester ::emitOneAndCheck(NATIVE_UINT_TYPE expectedIndex,
                              QueueType expectedType,
                              Fw::ComBuffer& expectedCom,
                              Fw::Buffer& expectedBuff) {
    emitOne();

    if (expectedType == QueueType::COM_QUEUE) {
        ASSERT_from_comQueueSend(expectedIndex, expectedCom, 0);
    } else {
        ASSERT_from_buffQueueSend(expectedIndex, expectedBuff);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testQueueSend() {
    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        emitOneAndCheck(portNum, QueueType::COM_QUEUE, comBuffer, buffer);
    }
    clearFromPortHistory();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        emitOneAndCheck(portNum, QueueType::BUFFER_QUEUE, comBuffer, buffer);
    }
    clearFromPortHistory();
    component.cleanup();
}

void Tester ::testQueuePause() {
    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++){
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        // Send a bunch of failures
        Fw::Success state = Fw::Success::FAILURE;
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        emitOneAndCheck(portNum, QueueType::COM_QUEUE, comBuffer, buffer);
    }
    clearFromPortHistory();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++){
        invoke_to_buffQueueIn(portNum, buffer);
        // Send a bunch of failures
        Fw::Success state = Fw::Success::FAILURE;
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        emitOneAndCheck(portNum, QueueType::BUFFER_QUEUE, comBuffer, buffer);
    }
    clearFromPortHistory();
    component.cleanup();
}

void Tester ::testPrioritySend() {
    U8 data[ComQueue::TOTAL_PORT_COUNT][BUFFER_LENGTH];

    ComQueue::QueueConfigurationTable configurationTable;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++) {
        configurationTable.entries[i].priority = ComQueue::TOTAL_PORT_COUNT - i - 1;
        configurationTable.entries[i].depth = 3;
        data[i][0] = ComQueue::TOTAL_PORT_COUNT - i - 1;
    }

    // Make the last message have the same priority as the second message
    configurationTable.entries[ComQueue::TOTAL_PORT_COUNT - 1].priority = 1;
    data[ComQueue::TOTAL_PORT_COUNT - 2][0] = 0;
    data[ComQueue::TOTAL_PORT_COUNT - 1][0] = 1;

    component.configure(configurationTable, 0, mallocAllocator);

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++){
        Fw::ComBuffer comBuffer(&data[portNum][0], BUFFER_LENGTH);
        invoke_to_comQueueIn(portNum, comBuffer, 0);
    }

    for (NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        Fw::Buffer buffer(&data[portNum + ComQueue::COM_PORT_COUNT][0], BUFFER_LENGTH);
        invoke_to_buffQueueIn(portNum, buffer);
    }

    // Check that nothing has yet been sent
    ASSERT_from_buffQueueSend_SIZE(0);
    ASSERT_from_comQueueSend_SIZE(0);

    for (NATIVE_INT_TYPE index = 0; index < ComQueue::TOTAL_PORT_COUNT; index++) {
        U8 orderKey;
        U32 previousComSize = fromPortHistory_comQueueSend->size();
        U32 previousBufSize = fromPortHistory_buffQueueSend->size();
        emitOne();
        ASSERT_EQ(fromPortHistory_comQueueSend->size() + fromPortHistory_buffQueueSend->size(), (index + 1));

        // Check that the sizes changed by exactly one
        ASSERT_TRUE((previousComSize == fromPortHistory_comQueueSend->size()) ^
                    (previousBufSize == fromPortHistory_buffQueueSend->size()));

        // Look for which type had arrived
        if (fromPortHistory_comQueueSend->size() > previousComSize) {
            orderKey = fromPortHistory_comQueueSend->at(fromPortHistory_comQueueSend->size() - 1).data.getBuffAddr()[0];
        } else {
            orderKey =
                fromPortHistory_buffQueueSend->at(fromPortHistory_buffQueueSend->size() - 1).fwBuffer.getData()[0];

        }
        ASSERT_EQ(orderKey, index);
    }
    clearFromPortHistory();
    component.cleanup();
}

void Tester::testQueueOverflow(){
    ComQueue::QueueConfigurationTable configurationTable;
    ComQueueDepth expectedComDepth;
    BuffQueueDepth expectedBuffDepth;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++){
        configurationTable.entries[i].priority = i;
        configurationTable.entries[i].depth = 2;

        // Expected depths
        if (i < ComQueue::COM_PORT_COUNT) {
            expectedComDepth[i] = configurationTable.entries[i].depth;
        } else {
            expectedBuffDepth[i - ComQueue::COM_PORT_COUNT] = configurationTable.entries[i].depth;
        }
    }

    component.configure(configurationTable, 0, mallocAllocator);

    for(NATIVE_INT_TYPE queueNum = 0; queueNum < ComQueue::TOTAL_PORT_COUNT; queueNum++) {
        QueueType overflow_type;
        NATIVE_INT_TYPE portNum;
        // queue[portNum].depth + 2 to deliberately cause overflow and check throttle of exactly 1
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < configurationTable.entries[queueNum].depth + 2; msgCount++) {
            sendByQueueNumber(queueNum, portNum, overflow_type);
            dispatchAll();
        }
        ASSERT_EVENTS_QueueOverflow_SIZE(1);
        ASSERT_EVENTS_QueueOverflow(0, overflow_type, portNum);

        // Drain a message, and see if throttle resets
        emitOne();

        // Force another overflow by filling then deliberately overflowing the queue
        sendByQueueNumber(queueNum, portNum, overflow_type);
        sendByQueueNumber(queueNum, portNum, overflow_type);
        dispatchAll();

        ASSERT_EVENTS_QueueOverflow_SIZE(2);
        ASSERT_EVENTS_QueueOverflow(1, overflow_type, portNum);

        // Drain the queue again such that we have a clean slate before the next queue
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < configurationTable.entries[queueNum].depth; msgCount++) {
            emitOne();
        }
        clearEvents();
    }
    // Check max seen queue-depths
    invoke_to_run(0, 0);
    dispatchAll();
    ASSERT_TLM_comQueueDepth_SIZE(1);
    ASSERT_TLM_buffQueueDepth_SIZE(1);
    ASSERT_TLM_comQueueDepth(0, expectedComDepth);
    ASSERT_TLM_buffQueueDepth(0, expectedBuffDepth);
    component.cleanup();
}

void Tester ::testReadyFirst() {
    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++){
        emitOne();
        invoke_to_comQueueIn(portNum, comBuffer, 0);
        dispatchAll();
        ASSERT_from_comQueueSend(portNum, comBuffer, 0);
    }
    clearFromPortHistory();

    for(NATIVE_INT_TYPE portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++){
        emitOne();
        invoke_to_buffQueueIn(portNum, buffer);
        dispatchAll();
        ASSERT_from_buffQueueSend(portNum, buffer);
    }
    clearFromPortHistory();
    component.cleanup();
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
    for (NATIVE_INT_TYPE i = 0; i < ComQueue::BUFFER_PORT_COUNT; ++i) {
        this->connect_to_buffQueueIn(i, this->component.get_buffQueueIn_InputPort(i));
    }

    // comQueueIn
    for (NATIVE_INT_TYPE i = 0; i < ComQueue::COM_PORT_COUNT; ++i) {
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
