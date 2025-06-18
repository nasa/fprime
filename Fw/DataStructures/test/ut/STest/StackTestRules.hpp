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

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return state.stack.getSize() > 0; }
    void action(State& state) {
        const auto size = state.stack.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        ASSERT_EQ(state.stack.at(index), state.modelStack.at(size - 1 - index));
    }
};

extern At at;

struct PopOK : public Rule {
    PopOK() : Rule("PopOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.stack.getSize();
        U32 value = 0;
        const auto status = state.stack.pop(value);
        ASSERT_EQ(status, Success::SUCCESS);
        const auto expectedValue = state.modelStack.at(size - 1);
        ASSERT_EQ(value, expectedValue);
        state.modelStack.pop_back();
        ASSERT_EQ(state.stack.getSize(), state.modelStack.size());
    }
};

extern PopOK popOK;

struct PopEmpty : public Rule {
    PopEmpty() : Rule("PopEmpty") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.stack.getSize()) == 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.stack.pop(value);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

extern PopEmpty popEmpty;

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.stack.getSize() > 0; }
    void action(State& state) {
        state.stack.clear();
        ASSERT_EQ(state.stack.getSize(), 0);
        state.modelStack.clear();
    }
};

extern Clear clear;

};  // namespace Rules

}  // namespace StackTest

}  // namespace Fw

#endif
