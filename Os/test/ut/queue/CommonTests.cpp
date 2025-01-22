// ======================================================================
// \title Os/test/ut/queue/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/queue/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Queue.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "Os/test/ut/queue/RulesHeaders.hpp"

const FwSizeType RANDOM_BOUND = 10000;

namespace Os {
namespace Test {
namespace Queue {
FwSizeType Tester::QueueState::queues = 0;

PriorityCompare const Tester::QueueMessageComparer::HELPER = PriorityCompare();

Os::QueueInterface::Status Tester::shadow_create(FwSizeType depth, FwSizeType messageSize) {
    Os::QueueInterface::Status status = Os::QueueInterface::ALREADY_CREATED;
    if (not this->shadow.created) {
        this->shadow.depth = depth;
        this->shadow.messageSize = messageSize;
        this->shadow.created = true;
        status = Os::QueueInterface::OP_OK;
        Tester::QueueState::queues++;
    }
    return status;
}

Os::QueueInterface::Status Tester::shadow_send(const U8* buffer,
                                               FwSizeType size,
                                               FwQueuePriorityType priority,
                                               Os::QueueInterface::BlockingType blockType) {
    this->shadow_check();
    QueueMessage qm;
    qm.priority = priority;
    qm.size = size;
    std::memcpy(qm.data, buffer, size);
    if (size > this->shadow.messageSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    } else if ((this->shadow.queue.size() == this->shadow.depth) &&
               (blockType == Os::QueueInterface::BlockingType::BLOCKING)) {
        this->shadow.send_block = qm;
        return QueueInterface::Status::OP_OK;
    } else if (this->shadow.queue.size() == this->shadow.depth) {
        return QueueInterface::Status::FULL;
    } else {
        this->shadow.queue.push(qm);
        this->shadow.highMark = FW_MAX(this->shadow.highMark, this->shadow.queue.size());
        return QueueInterface::Status::OP_OK;
    }
    return QueueInterface::Status::OP_OK;
}

void Tester::shadow_send_unblock() {
    // Send the shadow send buffered message
    this->shadow.queue.push(this->shadow.send_block);
    this->shadow.highMark = FW_MAX(this->shadow.highMark, this->shadow.queue.size());
}

Os::QueueInterface::Status Tester::shadow_receive(U8* destination,
                                                  FwSizeType capacity,
                                                  QueueInterface::BlockingType blockType,
                                                  FwSizeType& actualSize,
                                                  FwQueuePriorityType& priority) {
    this->shadow_check();
    if (capacity < this->shadow.messageSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    } else if (this->shadow.queue.empty() && (blockType == QueueInterface::BlockingType::BLOCKING)) {
        this->shadow.receive_block.destination = destination;
        this->shadow.receive_block.size = &actualSize;
        this->shadow.receive_block.priority = &priority;
        return QueueInterface::Status::OP_OK;
    } else if (this->shadow.queue.empty()) {
        return QueueInterface::Status::EMPTY;
    } else {
        const QueueMessage& qm = shadow.queue.top();
        std::memcpy(destination, qm.data, qm.size);
        actualSize = qm.size;
        priority = qm.priority;
        shadow.queue.pop();
    }
    return QueueInterface::Status::OP_OK;
}

void Tester::shadow_receive_unblock() {
    // Make sure outputs were stored in the shadow receive buffer
    ASSERT_NE(this->shadow.receive_block.destination, nullptr);
    ASSERT_NE(this->shadow.receive_block.size, nullptr);
    ASSERT_NE(this->shadow.receive_block.priority, nullptr);

    // Fill the outputs stored in the shadow receive buffer
    const QueueMessage& qm = shadow.queue.top();
    std::memcpy(this->shadow.receive_block.destination, qm.data, qm.size);
    *this->shadow.receive_block.size = qm.size;
    *this->shadow.receive_block.priority = qm.priority;
    shadow.queue.pop();

    // Clear the shadow receive buffer
    this->shadow.receive_block.destination = nullptr;
    this->shadow.receive_block.size = nullptr;
    this->shadow.receive_block.priority = nullptr;
}

}  // namespace Queue
}  // namespace Test
}  // namespace Os

TEST(InterfaceUninitialized, SendPointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    U8 buffer[messageSize];

    Os::QueueInterface::Status status =
        queue.send(buffer, sizeof buffer, priority, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitialized, SendBuffer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    U8 storage[messageSize];
    Fw::ExternalSerializeBuffer buffer(storage, sizeof storage);

    Os::QueueInterface::Status status = queue.send(buffer, priority, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitialized, ReceivePointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    FwSizeType size = 200;
    FwQueuePriorityType priority;
    U8 storage[size];

    Os::QueueInterface::Status status =
        queue.receive(storage, sizeof storage, Os::QueueInterface::BlockingType::NONBLOCKING, size, priority);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitialized, ReceiveBuffer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    FwSizeType size = 200;
    FwQueuePriorityType priority;
    U8 storage[size];
    Fw::ExternalSerializeBuffer buffer(storage, sizeof storage);

    Os::QueueInterface::Status status = queue.receive(buffer, Os::QueueInterface::BlockingType::NONBLOCKING, priority);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceInvalid, CreateInvalidDepth) {
    Os::Queue queue;
    Fw::String name = "My queue";
    ASSERT_DEATH_IF_SUPPORTED(queue.create(name, 0, 10), "Assert:.*Queue\\.cpp");
}

TEST(InterfaceInvalid, CreateInvalidSize) {
    Os::Queue queue;
    Fw::String name = "My queue";
    ASSERT_DEATH_IF_SUPPORTED(queue.create(name, 10, 0), "Assert:.*Queue\\.cpp");
}

TEST(InterfaceInvalid, SendPointerNull) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    ASSERT_DEATH_IF_SUPPORTED(queue.send(nullptr, messageSize, priority, Os::QueueInterface::BlockingType::BLOCKING),
                              "Assert:.*Queue\\.cpp");
}

TEST(InterfaceInvalid, SendInvalidEnum) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    Os::QueueInterface::BlockingType blockingType =
        static_cast<Os::QueueInterface::BlockingType>(Os::QueueInterface::BlockingType::BLOCKING + 1);
    ASSERT_DEATH_IF_SUPPORTED(queue.send(nullptr, messageSize, priority, blockingType), "Assert:.*Queue\\.cpp");
}

TEST(InterfaceInvalid, ReceivePointerNull) {
    Os::Queue queue;
    Fw::String name = "My queue";
    FwSizeType size = 200;
    FwQueuePriorityType priority;
    ASSERT_DEATH_IF_SUPPORTED(
        queue.receive(nullptr, size, Os::QueueInterface::BlockingType::NONBLOCKING, size, priority),
        "Assert:.*Queue\\.cpp");
}

TEST(InterfaceInvalid, ReceiveInvalidEnum) {
    Os::Queue queue;
    Fw::String name = "My queue";
    FwSizeType size = 200;
    FwQueuePriorityType priority;
    Os::QueueInterface::BlockingType blockingType =
        static_cast<Os::QueueInterface::BlockingType>(Os::QueueInterface::BlockingType::BLOCKING + 1);
    ASSERT_DEATH_IF_SUPPORTED(queue.receive(nullptr, size, blockingType, size, priority), "Assert:.*Queue\\.cpp");
}

TEST(BasicRules, Create) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    // Nominal create
    create_rule.action(tester);
    // Repetitive create
    create_rule.action(tester);
}

TEST(BasicRules, Send) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::SendNotFull send_rule;
    create_rule.action(tester);
    send_rule.action(tester);
}

TEST(BasicRules, Overflow) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    create_rule.action(tester);
    overflow_rule.action(tester);
}

TEST(BasicRules, Recv) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::ReceiveEmptyNoBlock receive_rule;

    create_rule.action(tester);
    receive_rule.action(tester);
}

TEST(BasicRules, Underflow) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Underflow underflow_rule;
    create_rule.action(tester);
    underflow_rule.action(tester);
}

TEST(BasicRules, SendRecv) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::SendNotFull send_rule;
    Os::Test::Queue::Tester::ReceiveNotEmpty receive_rule;

    create_rule.action(tester);
    send_rule.action(tester);
    receive_rule.action(tester);
}

TEST(BasicRules, OverflowUnderflow) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    Os::Test::Queue::Tester::SendFullNoBlock send_full_no_block;
    Os::Test::Queue::Tester::Underflow underflow_rule;
    Os::Test::Queue::Tester::ReceiveEmptyNoBlock receive_empty_no_block;
    create_rule.action(tester);
    overflow_rule.action(tester);
    send_full_no_block.action(tester);
    underflow_rule.action(tester);
    receive_empty_no_block.apply(tester);
}

TEST(Blocking, SendBlock) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    AggregatedConcurrentRule<Os::Test::Queue::Tester> aggregator;
    Os::Test::Queue::Tester::SendBlock block(aggregator);
    Os::Test::Queue::Tester::ReceiveNotEmpty receive_not_empty(false);
    ConcurrentWrapperRule<Os::Test::Queue::Tester> unblock(aggregator, receive_not_empty, "SendBlock", "SendUnblock");

    create_rule.apply(tester);
    overflow_rule.apply(tester);
    aggregator.apply(tester);
    aggregator.join();
}

TEST(Blocking, ReceiveBlock) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Underflow overflow_rule;
    AggregatedConcurrentRule<Os::Test::Queue::Tester> aggregator;
    Os::Test::Queue::Tester::ReceiveBlock block(aggregator);
    Os::Test::Queue::Tester::SendNotFull send_not_full(false);
    ConcurrentWrapperRule<Os::Test::Queue::Tester> unblock(aggregator, send_not_full, "ReceiveBlock", "ReceiveUnblock");

    create_rule.apply(tester);
    overflow_rule.apply(tester);
    aggregator.apply(tester);
    aggregator.join();
}

TEST(Random, RandomNominal) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::SendNotFull send_rule;
    Os::Test::Queue::Tester::SendFullNoBlock send_full_no_block_rule;
    Os::Test::Queue::Tester::ReceiveNotEmpty receive_rule;
    Os::Test::Queue::Tester::ReceiveEmptyNoBlock receive_empty_no_block_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    Os::Test::Queue::Tester::Underflow underflow_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::Queue::Tester>* rules[] = {&create_rule,
                                                     &send_rule,
                                                     &receive_rule,
                                                     &overflow_rule,
                                                     &underflow_rule,
                                                     &send_full_no_block_rule,
                                                     &receive_empty_no_block_rule};

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::Queue::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::Queue::Tester> bounded("Bounded Random Rules Scenario", random, RANDOM_BOUND);
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}
