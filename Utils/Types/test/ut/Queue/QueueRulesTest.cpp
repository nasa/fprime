// ======================================================================
// \title  QueueRulesTest.cpp
// \brief  Rules-based (STest) unit tests for Types::Queue
//
// Runs random scenarios against a ground-truth std::deque model for every
// combination of queue mode (FIFO/LIFO) and overflow mode
// (DROP_NEWEST/DROP_OLDEST).
// ======================================================================

#include <gtest/gtest.h>
#include <STest/STest/Pick/Pick.hpp>
#include <STest/STest/Rule/Rule.hpp>
#include <STest/Scenario/BoundedScenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <Utils/Types/Queue.hpp>
#include <algorithm>
#include <deque>

namespace {

constexpr FwSizeType MSG_SIZE = sizeof(U32);
constexpr FwSizeType QUEUE_DEPTH = 5;
constexpr U32 STEP_COUNT = 10000;

struct QueueState {
    QueueState(Types::QueueMode queueMode, Types::QueueOverflowMode queueOverflowMode)
        : mode(queueMode), overflowMode(queueOverflowMode) {
        queue.setup(storage, sizeof(storage), QUEUE_DEPTH, MSG_SIZE, mode, overflowMode);
    }

    Types::Queue queue;
    U8 storage[MSG_SIZE * QUEUE_DEPTH] = {};
    Types::QueueMode mode;
    Types::QueueOverflowMode overflowMode;
    std::deque<U32> truth;
    FwSizeType highWater = 0;
};

U32 randomMessage() {
    return (STest::Pick::lowerUpper(0, 0xFFFF) << 16) | STest::Pick::lowerUpper(0, 0xFFFF);
}

struct EnqueueOkRule : public STest::Rule<QueueState> {
    EnqueueOkRule() : STest::Rule<QueueState>("EnqueueOk") {}
    bool precondition(const QueueState& state) override { return state.truth.size() < QUEUE_DEPTH; }
    void action(QueueState& state) override {
        const U32 message = randomMessage();
        ASSERT_EQ(state.queue.enqueue(reinterpret_cast<const U8*>(&message), MSG_SIZE), Fw::FW_SERIALIZE_OK);
        state.truth.push_back(message);
        state.highWater = std::max(state.highWater, static_cast<FwSizeType>(state.truth.size()));
    }
};

struct EnqueueFullDropNewestRule : public STest::Rule<QueueState> {
    EnqueueFullDropNewestRule() : STest::Rule<QueueState>("EnqueueFullDropNewest") {}
    bool precondition(const QueueState& state) override {
        return state.truth.size() == QUEUE_DEPTH && state.overflowMode == Types::QUEUE_DROP_NEWEST;
    }
    void action(QueueState& state) override {
        const U32 message = randomMessage();
        ASSERT_EQ(state.queue.enqueue(reinterpret_cast<const U8*>(&message), MSG_SIZE), Fw::FW_SERIALIZE_NO_ROOM_LEFT);
        // Truth model unchanged: the new message was rejected
    }
};

struct EnqueueFullDropOldestRule : public STest::Rule<QueueState> {
    EnqueueFullDropOldestRule() : STest::Rule<QueueState>("EnqueueFullDropOldest") {}
    bool precondition(const QueueState& state) override {
        return state.truth.size() == QUEUE_DEPTH && state.overflowMode == Types::QUEUE_DROP_OLDEST;
    }
    void action(QueueState& state) override {
        const U32 message = randomMessage();
        ASSERT_EQ(state.queue.enqueue(reinterpret_cast<const U8*>(&message), MSG_SIZE),
                  Fw::FW_SERIALIZE_DISCARDED_EXISTING);
        state.truth.pop_front();
        state.truth.push_back(message);
        // The internal serialize refills the queue to capacity, updating the high-water mark
        state.highWater = std::max(state.highWater, QUEUE_DEPTH);
    }
};

struct DequeueOkRule : public STest::Rule<QueueState> {
    DequeueOkRule() : STest::Rule<QueueState>("DequeueOk") {}
    bool precondition(const QueueState& state) override { return !state.truth.empty(); }
    void action(QueueState& state) override {
        U32 message = 0;
        ASSERT_EQ(state.queue.dequeue(reinterpret_cast<U8*>(&message), MSG_SIZE), Fw::FW_SERIALIZE_OK);
        if (state.mode == Types::QUEUE_FIFO) {
            ASSERT_EQ(message, state.truth.front());
            state.truth.pop_front();
        } else {
            ASSERT_EQ(message, state.truth.back());
            state.truth.pop_back();
        }
    }
};

struct DequeueEmptyRule : public STest::Rule<QueueState> {
    DequeueEmptyRule() : STest::Rule<QueueState>("DequeueEmpty") {}
    bool precondition(const QueueState& state) override { return state.truth.empty(); }
    void action(QueueState& state) override {
        U32 message = 0;
        ASSERT_EQ(state.queue.dequeue(reinterpret_cast<U8*>(&message), MSG_SIZE), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    }
};

struct PopFrontOkRule : public STest::Rule<QueueState> {
    PopFrontOkRule() : STest::Rule<QueueState>("PopFrontOk") {}
    bool precondition(const QueueState& state) override { return !state.truth.empty(); }
    void action(QueueState& state) override {
        U32 message = 0;
        ASSERT_EQ(state.queue.popFront(reinterpret_cast<U8*>(&message), MSG_SIZE), Fw::FW_SERIALIZE_OK);
        // popFront always removes the oldest message regardless of queue mode
        ASSERT_EQ(message, state.truth.front());
        state.truth.pop_front();
    }
};

struct PopFrontEmptyRule : public STest::Rule<QueueState> {
    PopFrontEmptyRule() : STest::Rule<QueueState>("PopFrontEmpty") {}
    bool precondition(const QueueState& state) override { return state.truth.empty(); }
    void action(QueueState& state) override {
        U32 message = 0;
        ASSERT_EQ(state.queue.popFront(reinterpret_cast<U8*>(&message), MSG_SIZE), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    }
};

struct CheckStateRule : public STest::Rule<QueueState> {
    CheckStateRule() : STest::Rule<QueueState>("CheckState") {}
    bool precondition(const QueueState& state) override {
        static_cast<void>(state);
        return true;
    }
    void action(QueueState& state) override {
        ASSERT_EQ(state.queue.getQueueSize(), state.truth.size());
        ASSERT_EQ(state.queue.get_high_water_mark(), state.highWater);
        if (STest::Pick::lowerUpper(0, 9) == 0) {
            state.queue.clear_high_water_mark();
            state.highWater = 0;
        }
    }
};

void runScenario(Types::QueueMode mode, Types::QueueOverflowMode overflowMode) {
    QueueState state(mode, overflowMode);

    EnqueueOkRule enqueueOk;
    EnqueueFullDropNewestRule enqueueFullDropNewest;
    EnqueueFullDropOldestRule enqueueFullDropOldest;
    DequeueOkRule dequeueOk;
    DequeueEmptyRule dequeueEmpty;
    PopFrontOkRule popFrontOk;
    PopFrontEmptyRule popFrontEmpty;
    CheckStateRule checkState;

    STest::Rule<QueueState>* rules[] = {&enqueueOk,
                                        &enqueueFullDropNewest,
                                        &enqueueFullDropOldest,
                                        &dequeueOk,
                                        &dequeueEmpty,
                                        &popFrontOk,
                                        &popFrontEmpty,
                                        &checkState};

    STest::RandomScenario<QueueState> random("RandomQueueRules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<QueueState> bounded("BoundedRandomQueueRules", random, STEP_COUNT);
    const U32 numSteps = bounded.run(state);
    ASSERT_EQ(numSteps, STEP_COUNT);
}

TEST(QueueRulesTest, FifoDropNewest) {
    runScenario(Types::QUEUE_FIFO, Types::QUEUE_DROP_NEWEST);
}

TEST(QueueRulesTest, FifoDropOldest) {
    runScenario(Types::QUEUE_FIFO, Types::QUEUE_DROP_OLDEST);
}

TEST(QueueRulesTest, LifoDropNewest) {
    runScenario(Types::QUEUE_LIFO, Types::QUEUE_DROP_NEWEST);
}

TEST(QueueRulesTest, LifoDropOldest) {
    runScenario(Types::QUEUE_LIFO, Types::QUEUE_DROP_OLDEST);
}

}  // namespace
