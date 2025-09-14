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
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
}

void ComQueueTester ::configure() {
    ComQueue::QueueConfigurationTable configurationTable;
    for (FwIndexType i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++) {
        configurationTable.entries[i].priority = i;
        configurationTable.entries[i].depth = 3;
    }
    component.configure(configurationTable, 0, mallocAllocator);
}

void ComQueueTester ::sendByQueueNumber(Fw::Buffer& buffer,
                                        FwIndexType queueNum,
                                        FwIndexType& portNum,
                                        QueueType& queueType) {
    if (queueNum < ComQueue::COM_PORT_COUNT) {
        Fw::ComBuffer comBuffer(buffer.getData(), buffer.getSize());
        portNum = queueNum;
        queueType = QueueType::COM_QUEUE;
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
    } else {
        portNum = queueNum - ComQueue::COM_PORT_COUNT;
        queueType = QueueType::BUFFER_QUEUE;
        invoke_to_bufferQueueIn(portNum, buffer);
    }
}

void ComQueueTester ::emitOne() {
    Fw::Success state = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, state);
    dispatchAll();
}

void ComQueueTester ::emitOneAndCheck(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedSize) {
    emitOne();
    // Check that the data buffers are identical (size + data)
    Fw::Buffer emittedBuffer = this->fromPortHistory_dataOut->at(expectedIndex).data;
    ASSERT_EQ(expectedSize, emittedBuffer.getSize());
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(emittedBuffer.getData()[i], expectedData[i]);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComQueueTester ::testQueueSend() {
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for (FwIndexType portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++) {
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
        emitOneAndCheck(portNum, comBuffer.getBuffAddr(), comBuffer.getBuffLength());
    }
    clearFromPortHistory();

    for (FwIndexType portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        invoke_to_bufferQueueIn(portNum, buffer);
        emitOneAndCheck(portNum, buffer.getData(), buffer.getSize());
    }
    clearFromPortHistory();
    component.cleanup();
}

void ComQueueTester ::testQueuePause() {
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for (FwIndexType portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++) {
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
        // Send a bunch of failures
        Fw::Success state = Fw::Success::FAILURE;
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        emitOneAndCheck(portNum, comBuffer.getBuffAddr(), comBuffer.getBuffLength());
    }
    clearFromPortHistory();

    for (FwIndexType portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        invoke_to_bufferQueueIn(portNum, buffer);
        // Send a bunch of failures
        Fw::Success state = Fw::Success::FAILURE;
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        invoke_to_comStatusIn(0, state);
        emitOneAndCheck(portNum, buffer.getData(), buffer.getSize());
    }
    clearFromPortHistory();
    component.cleanup();
}

void ComQueueTester ::testPrioritySend() {
    U8 data[ComQueue::TOTAL_PORT_COUNT][BUFFER_LENGTH] = {};

    ComQueue::QueueConfigurationTable configurationTable;

    for (FwIndexType i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++) {
        configurationTable.entries[i].priority = ComQueue::TOTAL_PORT_COUNT - i - 1;
        configurationTable.entries[i].depth = 3;
        // Set a unique data value in the buffer (at offset, accounting for mandatory packet type)
        data[i][BUFFER_DATA_OFFSET] = ComQueue::TOTAL_PORT_COUNT - i - 1;
    }

    // Make the last message have the same priority as the second message
    configurationTable.entries[ComQueue::TOTAL_PORT_COUNT - 1].priority = 1;
    data[ComQueue::TOTAL_PORT_COUNT - 2][BUFFER_DATA_OFFSET] = 0;
    data[ComQueue::TOTAL_PORT_COUNT - 1][BUFFER_DATA_OFFSET] = 1;

    component.configure(configurationTable, 0, mallocAllocator);

    for (FwIndexType portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++) {
        Fw::ComBuffer comBuffer(&data[portNum][0], BUFFER_LENGTH);
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
    }

    for (FwIndexType portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        Fw::Buffer buffer(&data[portNum + ComQueue::COM_PORT_COUNT][0], BUFFER_LENGTH);
        invoke_to_bufferQueueIn(portNum, buffer);
    }

    // Check that nothing has yet been sent
    ASSERT_from_dataOut_SIZE(0);

    for (FwIndexType index = 0; index < ComQueue::TOTAL_PORT_COUNT; index++) {
        U8 orderKey;
        U32 previousSize = fromPortHistory_dataOut->size();
        emitOne();
        ASSERT_EQ(fromPortHistory_dataOut->size(), (index + 1));
        // Check that the size changed by exactly one
        ASSERT_EQ(fromPortHistory_dataOut->size(), (previousSize + 1));

        orderKey = fromPortHistory_dataOut->at(index).data.getData()[BUFFER_DATA_OFFSET];
        ASSERT_EQ(orderKey, index);
    }
    clearFromPortHistory();
    component.cleanup();
}

void ComQueueTester::testExternalQueueOverflow() {
    // "External" queue is ComQueue's managed queue for input Com/Buffers
    // as opposed to the "internal" message queue for async input ports
    ComQueue::QueueConfigurationTable configurationTable;
    ComQueueDepth expectedComDepth;
    BuffQueueDepth expectedBuffDepth;

    for (FwIndexType i = 0; i < ComQueue::TOTAL_PORT_COUNT; i++) {
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

    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::Buffer buffer(&data[0], sizeof(data));

    for (FwIndexType queueNum = 0; queueNum < ComQueue::TOTAL_PORT_COUNT; queueNum++) {
        QueueType overflow_type;
        FwIndexType portNum;
        // queue[portNum].depth + 2 to deliberately cause overflow of 2, in order to also test the throttle
        for (FwSizeType msgCount = 0; msgCount < configurationTable.entries[queueNum].depth + 2; msgCount++) {
            sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
            dispatchAll();
        }
        // Throttle should make it that we emitted only 1 event, even though we overflowed twice
        ASSERT_EVENTS_QueueOverflow_SIZE(1);
        ASSERT_EVENTS_QueueOverflow(0, overflow_type, portNum);

        if (QueueType::BUFFER_QUEUE == overflow_type) {
            // Two messages overflowed, so two buffers should be returned
            ASSERT_from_bufferReturnOut_SIZE(2);
            ASSERT_from_bufferReturnOut(0, buffer);
            ASSERT_from_bufferReturnOut(1, buffer);
        }

        // Drain a message, and see if throttle resets
        emitOne();

        // Force another overflow by filling then deliberately overflowing the queue
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        dispatchAll();

        if (QueueType::BUFFER_QUEUE == overflow_type) {
            // Third message overflowed, so third bufferReturnOut
            ASSERT_from_bufferReturnOut_SIZE(3);
            ASSERT_from_bufferReturnOut(2, buffer);
        }

        // emitOne() reset the throttle, then overflow again. So expect a second overflow event
        ASSERT_EVENTS_QueueOverflow_SIZE(2);
        ASSERT_EVENTS_QueueOverflow(1, overflow_type, portNum);

        // Drain the queue again such that we have a clean slate before the next queue
        for (FwSizeType msgCount = 0; msgCount < configurationTable.entries[queueNum].depth; msgCount++) {
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
    // Internal queue is the message queue for async input ports
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::Buffer buffer(data, sizeof(data));

    const FwIndexType queueNum = ComQueue::COM_PORT_COUNT;
    const FwSizeType msgCountMax = this->component.m_queue.getDepth();
    QueueType overflow_type;
    FwIndexType portNum;

    // fill the queue
    for (FwSizeType msgCount = 0; msgCount < msgCountMax; msgCount++) {
        sendByQueueNumber(buffer, queueNum, portNum, overflow_type);
        ASSERT_EQ(overflow_type, QueueType::BUFFER_QUEUE);
    }

    // send one more to overflow the queue
    sendByQueueNumber(buffer, queueNum, portNum, overflow_type);

    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer);

    // send another
    sendByQueueNumber(buffer, queueNum, portNum, overflow_type);

    ASSERT_from_bufferReturnOut_SIZE(2);
    ASSERT_from_bufferReturnOut(0, buffer);
    ASSERT_from_bufferReturnOut(1, buffer);

    component.cleanup();
}

void ComQueueTester ::testReadyFirst() {
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for (FwIndexType portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++) {
        emitOne();
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
        dispatchAll();

        Fw::Buffer emittedBuffer = this->fromPortHistory_dataOut->at(portNum).data;
        ASSERT_EQ(emittedBuffer.getSize(), comBuffer.getBuffLength());
        for (FwSizeType i = 0; i < emittedBuffer.getSize(); i++) {
            ASSERT_EQ(emittedBuffer.getData()[i], comBuffer.getBuffAddr()[i]);
        }
    }
    clearFromPortHistory();

    for (FwIndexType portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        emitOne();
        invoke_to_bufferQueueIn(portNum, buffer);
        dispatchAll();
        Fw::Buffer emittedBuffer = this->fromPortHistory_dataOut->at(portNum).data;
        ASSERT_EQ(emittedBuffer.getSize(), buffer.getSize());
        for (FwSizeType i = 0; i < buffer.getSize(); i++) {
            ASSERT_EQ(buffer.getData()[i], emittedBuffer.getData()[i]);
        }
    }
    clearFromPortHistory();
    component.cleanup();
}

void ComQueueTester ::testContextData() {
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::ComBuffer comBuffer(&data[0], sizeof(data));
    Fw::Buffer buffer(&data[0], sizeof(data));
    configure();

    for (FwIndexType portNum = 0; portNum < ComQueue::COM_PORT_COUNT; portNum++) {
        invoke_to_comPacketQueueIn(portNum, comBuffer, 0);
        emitOne();
        // Currently, the APID is set to the queue index, which is the same as the port number for COM ports
        FwIndexType expectedApid = portNum;
        auto emittedContext = this->fromPortHistory_dataOut->at(portNum).context;
        ASSERT_EQ(expectedApid, emittedContext.get_comQueueIndex());
    }
    clearFromPortHistory();

    for (FwIndexType portNum = 0; portNum < ComQueue::BUFFER_PORT_COUNT; portNum++) {
        invoke_to_bufferQueueIn(portNum, buffer);
        emitOne();
        // APID is queue index, which is COM_PORT_COUNT + portNum for BUFFER ports
        FwIndexType expectedApid = portNum + ComQueue::COM_PORT_COUNT;
        auto emittedContext = this->fromPortHistory_dataOut->at(portNum).context;
        ASSERT_EQ(expectedApid, emittedContext.get_comQueueIndex());
    }
    clearFromPortHistory();
    component.cleanup();
}

void ComQueueTester ::testBufferQueueReturn() {
    U8 data[BUFFER_LENGTH] = BUFFER_DATA;
    Fw::Buffer buffer(&data[0], sizeof(data));
    ComCfg::FrameContext context;
    configure();

    for (FwIndexType portNum = 0; portNum < ComQueue::TOTAL_PORT_COUNT; portNum++) {
        clearFromPortHistory();
        context.set_comQueueIndex(portNum);
        invoke_to_dataReturnIn(0, buffer, context);
        // APIDs that correspond to an buffer originating from a Fw.Com port
        // do no get deallocated – APIDs that correspond to a Fw.Buffer do
        if (portNum < ComQueue::COM_PORT_COUNT) {
            ASSERT_from_bufferReturnOut_SIZE(0);
        } else {
            ASSERT_from_bufferReturnOut_SIZE(1);
            ASSERT_from_bufferReturnOut(0, buffer);
        }
    }
    component.cleanup();
}

}  // end namespace Svc
