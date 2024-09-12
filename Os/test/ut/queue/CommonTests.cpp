// ======================================================================
// \title Os/test/ut/queue/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/queue/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Queue.hpp"
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

Os::QueueInterface::Status Tester::shadow_send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, Os::QueueInterface::BlockingType blockType) {
    this->shadow_check();
    if (this->shadow.queue.size() == this->shadow.depth) {
        return QueueInterface::Status::FULL;
    } else if (size > this->shadow.messageSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    QueueMessage qm;
    qm.priority = priority;
    qm.size = size;
    std::memcpy(qm.data, buffer, size);
    this->shadow.queue.push(qm);
    this->shadow.hwm = FW_MAX(this->shadow.hwm, this->shadow.queue.size());
    return QueueInterface::Status::OP_OK;
}

Os::QueueInterface::Status Tester::shadow_receive(U8* destination, FwSizeType capacity, QueueInterface::BlockingType blockType, FwSizeType& actualSize, FwQueuePriorityType& priority) {
    this->shadow_check();
    if (this->shadow.queue.empty()) {
        return QueueInterface::Status::EMPTY;
    } else if (capacity < this->shadow.messageSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    const QueueMessage& qm = shadow.queue.top();

    std::memcpy(destination, qm.data, qm.size);
    actualSize = qm.size;
    priority = qm.priority;
    shadow.queue.pop();
    return QueueInterface::Status::OP_OK;
}

} // namespace Os
} // namespace Test
} // namespace Queue


TEST(InterfaceUninitalized, SendPointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    U8 buffer[messageSize];

    Os::QueueInterface::Status status =
        queue.send(buffer, sizeof buffer, priority, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitalized, SendBuffer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType messageSize = 200;
    const FwQueuePriorityType priority = 300;
    U8 storage[messageSize];
    Fw::ExternalSerializeBuffer buffer(storage, sizeof storage);

    Os::QueueInterface::Status status = queue.send(buffer, priority, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitalized, ReceivePointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    FwSizeType size = 200;
    FwQueuePriorityType priority;
    U8 storage[size];

    Os::QueueInterface::Status status =
        queue.receive(storage, sizeof storage, Os::QueueInterface::BlockingType::NONBLOCKING, size, priority);
    ASSERT_EQ(Os::QueueInterface::Status::UNINITIALIZED, status);
}

TEST(InterfaceUninitalized, ReceiveBuffer) {
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
    ASSERT_DEATH_IF_SUPPORTED(queue.send(nullptr, messageSize, priority, blockingType),
                              "Assert:.*Queue\\.cpp");
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
    ASSERT_DEATH_IF_SUPPORTED(
        queue.receive(nullptr, size, blockingType, size, priority),
        "Assert:.*Queue\\.cpp");
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
    Os::Test::Queue::Tester::Send send_rule;
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
    Os::Test::Queue::Tester::Receive receive_rule;

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
    Os::Test::Queue::Tester::Send send_rule;
    Os::Test::Queue::Tester::Receive receive_rule;

    create_rule.action(tester);
    send_rule.action(tester);
    receive_rule.action(tester);
}

TEST(BasicRules, OverflowUnderflow) {
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    Os::Test::Queue::Tester::Underflow underflow_rule;
    create_rule.action(tester);
    overflow_rule.action(tester);
    underflow_rule.action(tester);
}

TEST(Random, RandomNominal) {
    STest::Random::SeedValue::set(882558);
    Os::Test::Queue::Tester tester;
    Os::Test::Queue::Tester::Create create_rule;
    Os::Test::Queue::Tester::Send send_rule;
    Os::Test::Queue::Tester::Receive receive_rule;
    Os::Test::Queue::Tester::Overflow overflow_rule;
    Os::Test::Queue::Tester::Underflow underflow_rule;


    // Place these rules into a list of rules
    STest::Rule<Os::Test::Queue::Tester>* rules[] = {
        &create_rule,
        &send_rule,
        &receive_rule,
        &overflow_rule,
        &underflow_rule,
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::Queue::Tester> random(
        "Random Rules",
        rules,
        FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::Queue::Tester> bounded(
        "Bounded Random Rules Scenario",
        random,
        RANDOM_BOUND
    );
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}