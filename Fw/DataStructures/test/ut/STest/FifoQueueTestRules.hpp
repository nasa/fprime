#ifndef FifoQueueTestRules_HPP
#define FifoQueueTestRules_HPP

#include <gtest/gtest.h>

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace FifoQueueTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct EnqueueOK : public Rule {
    EnqueueOK() : Rule("EnqueueOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.queue.getSize()) < State::capacity; }
    void action(State& state) {
        const U32 value = STest::Pick::any();
        const auto status = state.queue.enqueue(value);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelQueue.push_back(value);
    }
};
static EnqueueOK enqueueOK;

struct EnqueueFull : public Rule {
    EnqueueFull() : Rule("EnqueueFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.queue.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.queue.enqueue(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};
static EnqueueFull enqueueFull;

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return state.queue.getSize() > 0; }
    void action(State& state) {
        const auto index = STest::Pick::startLength(0, static_cast<U32>(state.queue.getSize()));
        ASSERT_EQ(state.queue.at(index), state.modelQueue.at(index));
    }
};
static At at;

struct DequeueOK : public Rule {
    DequeueOK() : Rule("DequeueOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.queue.getSize()) > 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.queue.dequeue(value);
        ASSERT_EQ(status, Success::SUCCESS);
        const auto expectedValue = state.modelQueue.at(0);
        ASSERT_EQ(value, expectedValue);
        state.modelQueue.pop_front();
        ASSERT_EQ(state.queue.getSize(), state.modelQueue.size());
    }
};
static DequeueOK dequeueOK;

struct DequeueEmpty : public Rule {
    DequeueEmpty() : Rule("DequeueEmpty") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.queue.getSize()) == 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.queue.dequeue(value);
        ASSERT_EQ(status, Success::FAILURE);
    }
};
static DequeueEmpty dequeueEmpty;

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.queue.getSize() > 0; }
    void action(State& state) {
        state.queue.clear();
        ASSERT_EQ(state.queue.getSize(), 0);
        state.modelQueue.clear();
    }
};
static Clear clear;
};  // namespace Rules

}  // namespace FifoQueueTest

}  // namespace Fw

#endif
