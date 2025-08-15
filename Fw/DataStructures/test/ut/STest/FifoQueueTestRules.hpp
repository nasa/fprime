// ======================================================================
// \title  FifoQueueTestRules.hpp
// \author bocchino
// \brief  hpp file for FIFO queue test rules
// ======================================================================

#ifndef FifoQueueTestRules_HPP
#define FifoQueueTestRules_HPP

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace FifoQueueTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return !state.queue.isEmpty(); }
    void action(State& state) {
        const auto index = STest::Pick::startLength(0, static_cast<U32>(state.queue.getSize()));
        ASSERT_EQ(state.queue.at(index), state.modelQueue.at(index));
    }
};

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return !state.queue.isEmpty(); }
    void action(State& state) {
        state.queue.clear();
        ASSERT_EQ(state.queue.getSize(), 0);
        state.modelQueue.clear();
    }
};

struct DequeueEmpty : public Rule {
    DequeueEmpty() : Rule("DequeueEmpty") {}
    bool precondition(const State& state) { return state.queue.isEmpty(); }
    void action(State& state) {
        State::ItemType item = 0;
        const auto status = state.queue.dequeue(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

struct DequeueOK : public Rule {
    DequeueOK() : Rule("DequeueOK") {}
    bool precondition(const State& state) { return !state.queue.isEmpty(); }
    void action(State& state) {
        State::ItemType item = 0;
        const auto status = state.queue.dequeue(item);
        ASSERT_EQ(status, Success::SUCCESS);
        const auto expectedItem = state.modelQueue.at(0);
        ASSERT_EQ(item, expectedItem);
        state.modelQueue.pop_front();
        ASSERT_EQ(state.queue.getSize(), state.modelQueue.size());
    }
};

struct EnqueueFull : public Rule {
    EnqueueFull() : Rule("EnqueueFull") {}
    bool precondition(const State& state) { return state.queue.isFull(); }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.queue.enqueue(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

struct EnqueueOK : public Rule {
    EnqueueOK() : Rule("EnqueueOK") {}
    bool precondition(const State& state) { return !state.queue.isFull(); }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.queue.enqueue(item);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelQueue.push_back(item);
    }
};

struct Peek : public Rule {
    Peek() : Rule("Peek") {}
    bool precondition(const State& state) { return !state.queue.isEmpty(); }
    void action(State& state) {
        const auto index = STest::Pick::startLength(0, static_cast<U32>(state.queue.getSize()));
        State::ItemType item = 0;
        const auto status = state.queue.peek(item, index);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, state.modelQueue.at(index));
    }
};

extern At at;

extern Clear clear;

extern DequeueEmpty dequeueEmpty;

extern DequeueOK dequeueOK;

extern EnqueueFull enqueueFull;

extern EnqueueOK enqueueOK;

extern Peek peek;

}  // namespace Rules

}  // namespace FifoQueueTest

}  // namespace Fw

#endif
