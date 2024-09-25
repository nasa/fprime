// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  cpp file for ComQueue test harness implementation class
// ======================================================================

#include "ComQueueTester.hpp"
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

ComQueueTester ::ComQueueTester() : ComQueueGTestBase("Tester", MAX_HISTORY_SIZE), component("ComQueue") {
    this->initComponents();
    this->connectPorts();
}

ComQueueTester ::~ComQueueTester() {}

void ComQueueTester ::dispatchAll() {
    while (this->component.m_queue.getNumMsgs() > 0) {
        this->component.doDispatch();
    }
}

void ComQueueTester ::configure() {
    ComQueue::QueueConfigurationTable configurationTable;
    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++){
        configurationTable.entries[i].priority = i;
        configurationTable.entries[i].depth = 3;
    }
    component.configure(configurationTable, 0, mallocAllocator);
}

void ComQueueTester ::sendByQueueNumber(Fw::Buffer& buffer,
                                        NATIVE_INT_TYPE queueNum,
                                        NATIVE_INT_TYPE& portNum,
                                        QueueType& queueType) {
    if (queueNum < ComQueue::COM_PORT_COUNT) {
        Fw::ComBuffer comBuffer(buffer.getData(), buffer.getSize());
        portNum = queueNum;
        queueType = QueueType::COM_QUEUE;
        invoke_to_comQueueIn(portNum, comBuffer, 0);
    } else {
        portNum = queueNum - ComQueue::COM_PORT_COUNT;
        queueType = QueueType::BUFFER_QUEUE;
        invoke_to_buffQueueIn(portNum, buffer);
    }
}

void ComQueueTester ::emitOne() {
    Fw::Success state = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, state);
    dispatchAll();
}

void ComQueueTester ::emitOneAndCheck(NATIVE_UINT_TYPE expectedIndex,
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

void ComQueueTester ::testQueueSend() {
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

void ComQueueTester ::testQueuePause() {
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

void ComQueueTester ::testPrioritySend() {
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

void ComQueueTester::testExternalQueueOverflow() {
    ComQueue::QueueConfigurationTable configurationTable;
    ComQueueDepth expectedComDepth;
    BuffQueueDepth expectedBuffDepth;

    for (NATIVE_UINT_TYPE i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++) {
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

    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::Buffer buffer(&data[0], sizeof(data));

    for (NATIVE_INT_TYPE queueNum = 0; queueNum < ComQueue::TOTAL_PORT_COUNT; queueNum++) {
        QueueType overflow_type;
        NATIVE_INT_TYPE portNum;
        // queue[portNum].depth + 2 to deliberately cause overflow and check throttle of exactly 1
        for (NATIVE_UINT_TYPE msgCount = 0; msgCount < configurationTable.entries[queueNum].depth + 2; msgCount++) {
            sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
            dispatchAll();
        }

        if (QueueType::BUFFER_QUEUE == overflow_type) {
            ASSERT_from_deallocate_SIZE(2);
            ASSERT_from_deallocate(0, buffer);
            ASSERT_from_deallocate(1, buffer);
        }

        ASSERT_EVENTS_QueueOverflow_SIZE(1);
        ASSERT_EVENTS_QueueOverflow(0, overflow_type, portNum);

        // Drain a message, and see if throttle resets
        emitOne();

        // Force another overflow by filling then deliberately overflowing the queue
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        dispatchAll();

        if (QueueType::BUFFER_QUEUE == overflow_type) {
            ASSERT_from_deallocate_SIZE(3);
            ASSERT_from_deallocate(2, buffer);
        }

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

void ComQueueTester::testInternalQueueOverflow() {
    U8 data[BUFFER_LENGTH] = {0xde, 0xad, 0xbe};
    Fw::Buffer buffer(data, sizeof(data));

    const NATIVE_INT_TYPE queueNum = ComQueue::COM_PORT_COUNT;
    const NATIVE_INT_TYPE msgCountMax = this->component.m_queue.getQueueSize();
    QueueType overflow_type;
    NATIVE_INT_TYPE portNum;

    // fill the queue
    for (NATIVE_INT_TYPE msgCount = 0; msgCount < msgCountMax; msgCount++) {
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        ASSERT_EQ(overflow_type, QueueType::BUFFER_QUEUE);
    }

    // send one more to overflow the queue
    sendByQueueNumber(buffer, queueNum, portNum, overflow_type);

    ASSERT_from_deallocate_SIZE(1);
    ASSERT_from_deallocate(0, buffer);

    // send another
    sendByQueueNumber(buffer, queueNum, portNum, overflow_type);

    ASSERT_from_deallocate_SIZE(2);
    ASSERT_from_deallocate(0, buffer);
    ASSERT_from_deallocate(1, buffer);

    component.cleanup();
}

void ComQueueTester ::testReadyFirst() {
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

void ComQueueTester ::from_buffQueueSend_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_buffQueueSend(fwBuffer);
}

void ComQueueTester ::from_comQueueSend_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comQueueSend(data, context);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

}  // end namespace Svc
