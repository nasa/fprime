// ======================================================================
// \title Os/test/ut/queue/QueueRules.cpp
// \brief queue rule implementations
// ======================================================================

#include "CommonTests.hpp"
#include "Fw/Types/String.hpp"

struct PickedMessage {
    FwSizeType size;
    FwQueuePriorityType priority;
    U8* sent;
    U8 received[QUEUE_MESSAGE_SIZE_UPPER_BOUND];
};

PickedMessage pick_message(FwSizeType max_size) {
    PickedMessage message;

    message.size = STest::Random::lowerUpper(1, max_size);
    message.priority = STest::Random::lowerUpper(0, std::numeric_limits<U32>::max());

    message.sent = new U8[message.size];
    for (FwSizeType i = 0; i < message.size; i++) {
        message.sent[i] = STest::Random::lowerUpper(0, std::numeric_limits<U8>::max());
    }
    return message;
}

void check_received(PickedMessage& message, PickedMessage& test) {
    EXPECT_EQ(message.size, test.size);
    EXPECT_EQ(message.priority, test.priority);
    for (FwSizeType i = 0; i < message.size; i++) {
        ASSERT_EQ(message.received[i], test.received[i]) << "at index " << i;
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Create
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Create::Create() : STest::Rule<Os::Test::Queue::Tester>("Create") {}

bool Os::Test::Queue::Tester::Create::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::Queue::Tester::Create::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    const bool created = state.is_shadow_created();
    Fw::String name("nAmE");
    FwSizeType depth = STest::Random::lowerUpper(1, QUEUE_DEPTH_UPPER_BOUND);
    FwSizeType messageSize = STest::Random::lowerUpper(1, QUEUE_MESSAGE_SIZE_UPPER_BOUND);
    QueueInterface::Status status = state.shadow_create(depth, messageSize);
    QueueInterface::Status test_status = state.queue.create(name, depth, messageSize);
    ASSERT_EQ(status, created ? QueueInterface::Status::ALREADY_CREATED : QueueInterface::Status::OP_OK);
    ASSERT_EQ(status, test_status);
    ASSERT_EQ(name, state.queue.getName());
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SendNotFull
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::SendNotFull::SendNotFull(bool end_check) : STest::Rule<Os::Test::Queue::Tester>("SendNotFull"),
    m_end_check(end_check)
{}

bool Os::Test::Queue::Tester::SendNotFull::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created and not state.is_shadow_full();
}

void Os::Test::Queue::Tester::SendNotFull::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    QueueInterface::BlockingType blocking =
        (STest::Random::lowerUpper(0, 1) == 1) ? QueueInterface::BLOCKING : QueueInterface::NONBLOCKING;
    PickedMessage pick = pick_message(state.shadow.messageSize);
    // Prevent lock-up
    ASSERT_LT(state.queue.getMessagesAvailable(), state.queue.getDepth());
    ASSERT_FALSE(state.is_shadow_full());
    QueueInterface::Status status = state.shadow_send(pick.sent, pick.size, pick.priority, blocking);
    QueueInterface::Status test_status = state.queue.send(pick.sent, pick.size, pick.priority, blocking);
    delete[] pick.sent;  // Clean-up
    ASSERT_EQ(status, QueueInterface::Status::OP_OK);
    ASSERT_EQ(test_status, status);
    if (this->m_end_check) {
        state.shadow_check();
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SendFullNoBlock
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::SendFullNoBlock::SendFullNoBlock() : STest::Rule<Os::Test::Queue::Tester>("SendFullNoBlock") {}

bool Os::Test::Queue::Tester::SendFullNoBlock::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created and state.is_shadow_full();
}

void Os::Test::Queue::Tester::SendFullNoBlock::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    PickedMessage pick = pick_message(state.shadow.messageSize);
    QueueInterface::Status status = state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::NONBLOCKING);
    QueueInterface::Status test_status =
        state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::NONBLOCKING);
    delete[] pick.sent;

    ASSERT_EQ(status, QueueInterface::Status::FULL);
    ASSERT_EQ(test_status, status);
    state.shadow_check();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveNotEmpty
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::ReceiveNotEmpty::ReceiveNotEmpty(bool end_check) : STest::Rule<Os::Test::Queue::Tester>("ReceiveNotEmpty"),
      m_end_check(end_check) {}

bool Os::Test::Queue::Tester::ReceiveNotEmpty::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created and not state.is_shadow_empty();
}

void Os::Test::Queue::Tester::ReceiveNotEmpty::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    QueueInterface::BlockingType blocking =
        (STest::Random::lowerUpper(0, 1) == 1) ? QueueInterface::BLOCKING : QueueInterface::NONBLOCKING;
    PickedMessage message;
    PickedMessage test;

    // Prevent lock-up
    ASSERT_GT(state.queue.getMessagesAvailable(), 0);
    ASSERT_FALSE(state.is_shadow_empty());
    QueueInterface::Status status = state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, blocking,
                                                         message.size, message.priority);

    QueueInterface::Status test_status =
        state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, blocking, test.size, test.priority);
    ASSERT_EQ(status, QueueInterface::Status::OP_OK);
    ASSERT_EQ(status, test_status);
    check_received(message, test);
    if (this->m_end_check) {
        state.shadow_check();
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveEmptyNoBlock
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::ReceiveEmptyNoBlock::ReceiveEmptyNoBlock()
    : STest::Rule<Os::Test::Queue::Tester>("ReceiveEmptyNoBlock") {}

bool Os::Test::Queue::Tester::ReceiveEmptyNoBlock::precondition(
    const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created and state.is_shadow_empty();
}

void Os::Test::Queue::Tester::ReceiveEmptyNoBlock::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    PickedMessage message;
    PickedMessage test;

    QueueInterface::Status status =
        state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                             QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

    QueueInterface::Status test_status =
        state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, QueueInterface::BlockingType::NONBLOCKING,
                            test.size, test.priority);
    ASSERT_EQ(status, QueueInterface::Status::EMPTY);
    ASSERT_EQ(status, test_status);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Overflow
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Overflow::Overflow() : STest::Rule<Os::Test::Queue::Tester>("Overflow") {}

bool Os::Test::Queue::Tester::Overflow::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.is_shadow_created();
}

void Os::Test::Queue::Tester::Overflow::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    while (not state.is_shadow_full()) {
        PickedMessage pick = pick_message(state.shadow.messageSize);
        QueueInterface::Status status =
            state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
        QueueInterface::Status test_status =
            state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
        delete[] pick.sent;
        ASSERT_EQ(status, QueueInterface::Status::OP_OK);
        ASSERT_EQ(status, test_status);
    }
    PickedMessage pick = pick_message(state.shadow.messageSize);
    QueueInterface::Status status =
        state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
    QueueInterface::Status test_status =
        state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
    delete[] pick.sent;
    ASSERT_EQ(status, QueueInterface::Status::FULL);
    ASSERT_EQ(status, test_status);
    state.shadow_check();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Underflow
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Underflow::Underflow() : STest::Rule<Os::Test::Queue::Tester>("Underflow") {}

bool Os::Test::Queue::Tester::Underflow::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.is_shadow_created();
}

void Os::Test::Queue::Tester::Underflow::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    PickedMessage message;
    PickedMessage test;
    while (not state.is_shadow_empty()) {
        QueueInterface::Status status =
            state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                                 QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

        QueueInterface::Status test_status =
            state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                                QueueInterface::BlockingType::NONBLOCKING, test.size, test.priority);
        ASSERT_EQ(status, QueueInterface::Status::OP_OK);
        ASSERT_EQ(status, test_status);
        check_received(message, test);
    }
    QueueInterface::Status status =
        state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                             QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

    QueueInterface::Status test_status =
        state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, QueueInterface::BlockingType::NONBLOCKING,
                            test.size, test.priority);
    ASSERT_EQ(status, QueueInterface::Status::EMPTY);
    ASSERT_EQ(status, test_status);
    state.shadow_check();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SendBlock
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::SendBlock::SendBlock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner)
    : ConcurrentRule<Os::Test::Queue::Tester>("SendBlock", runner) {}

bool Os::Test::Queue::Tester::SendBlock::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created && state.is_shadow_full();
}

void Os::Test::Queue::Tester::SendBlock::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    // Backend defined check
    if (not TESTS_SUPPORT_BLOCKING) {
        this->notify_other("SendUnblock");
        return;
    }
    PickedMessage pick = pick_message(state.shadow.messageSize);
    this->notify_other("SendUnblock");
    QueueInterface::Status status =
        state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::BLOCKING);
    getLock().unlock();
    QueueInterface::Status test_status =
        state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::BLOCKING);
    getLock().lock();
    // Condition should be set after block
    ASSERT_TRUE(this->getCondition());
    // Unblock the shadow queue send
    state.shadow_send_unblock();
    delete[] pick.sent;

    ASSERT_EQ(status, QueueInterface::Status::OP_OK);
    ASSERT_EQ(test_status, status);
    state.shadow_check();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveBlock
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::ReceiveBlock::ReceiveBlock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner)
    : ConcurrentRule<Os::Test::Queue::Tester>("ReceiveBlock", runner) {}

bool Os::Test::Queue::Tester::ReceiveBlock::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created && state.is_shadow_empty();
}

void Os::Test::Queue::Tester::ReceiveBlock::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    // Backend defined check
    if (not TESTS_SUPPORT_BLOCKING) {
        this->notify_other("ReceiveUnblock");
        return;
    }

    PickedMessage message;
    PickedMessage test;
    this->notify_other("ReceiveUnblock");

    QueueInterface::Status status =
        state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, QueueInterface::BlockingType::BLOCKING,
                             message.size, message.priority);
    this->getLock().unlock();
    QueueInterface::Status test_status =
        state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, QueueInterface::BlockingType::BLOCKING,
                            test.size, test.priority);
    this->getLock().lock();
    // Condition should be set after block
    ASSERT_TRUE(this->getCondition());
    // Unblock the shadow queue send
    state.shadow_receive_unblock();

    ASSERT_EQ(status, QueueInterface::Status::OP_OK);
    ASSERT_EQ(status, test_status);
    check_received(message, test);
}
