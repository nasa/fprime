// ======================================================================
// \title  StackTestRules.hpp
// \author bocchino
// \brief  hpp file for Stack test rules
// ======================================================================

#ifndef StackTestRules_HPP
#define StackTestRules_HPP

#include <gtest/gtest.h>

#include "Fw/DataStructures/test/ut/STest/StackTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace StackTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct PushOK : public Rule {
    PushOK() : Rule("PushOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) < State::capacity; }
    void action(State& state) {
        const U32 value = STest::Pick::any();
        const auto status = state.stack.push(value);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelStack.push_back(value);
    }
};

extern PushOK pushOK;

struct PushFull : public Rule {
    PushFull() : Rule("PushFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.stack.push(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

extern PushFull pushFull;

#if 0
struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return state.stack.getSize() > 0; }
    void action(State& state) {
        const auto index = STest::Pick::startLength(0, static_cast<U32>(state.stack.getSize()));
        ASSERT_EQ(state.stack.at(index), state.modelQueue.at(index));
    }
};

extern At at;

struct DestackOK : public Rule {
    DestackOK() : Rule("DestackOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) > 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.stack.pop(value);
        ASSERT_EQ(status, Success::SUCCESS);
        const auto expectedValue = state.modelQueue.at(0);
        ASSERT_EQ(value, expectedValue);
        state.modelQueue.pop_front();
        ASSERT_EQ(state.stack.getSize(), state.modelQueue.size());
    }
};

extern DestackOK popOK;

struct DestackEmpty : public Rule {
    DestackEmpty() : Rule("DestackEmpty") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) == 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.stack.pop(value);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

extern DestackEmpty popEmpty;

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.stack.getSize() > 0; }
    void action(State& state) {
        state.stack.clear();
        ASSERT_EQ(state.stack.getSize(), 0);
        state.modelQueue.clear();
    }
};

extern Clear clear;
#endif

};  // namespace Rules

}  // namespace StackTest

}  // namespace Fw

#endif
