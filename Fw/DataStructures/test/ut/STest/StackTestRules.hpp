// ======================================================================
// \title  StackTestRules.hpp
// \author bocchino
// \brief  hpp file for Stack test rules
// ======================================================================

#ifndef StackTestRules_HPP
#define StackTestRules_HPP

#include "Fw/DataStructures/test/ut/STest/StackTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace StackTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return !state.stack.isEmpty(); }
    void action(State& state) {
        const auto size = state.stack.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        ASSERT_EQ(state.stack.at(index), state.modelStack.at(size - 1 - index));
    }
};

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return !state.stack.isEmpty(); }
    void action(State& state) {
        state.stack.clear();
        ASSERT_EQ(state.stack.getSize(), 0);
        state.modelStack.clear();
    }
};

struct Peek : public Rule {
    Peek() : Rule("Peek") {}
    bool precondition(const State& state) { return !state.stack.isEmpty(); }
    void action(State& state) {
        const auto size = state.stack.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        State::ItemType item = 0;
        const auto status = state.stack.peek(item, index);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, state.modelStack.at(size - 1 - index));
    }
};

struct PopEmpty : public Rule {
    PopEmpty() : Rule("PopEmpty") {}
    bool precondition(const State& state) { return state.stack.isEmpty(); }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.stack.pop(value);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

struct PopOK : public Rule {
    PopOK() : Rule("PopOK") {}
    bool precondition(const State& state) { return !state.stack.isEmpty(); }
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

struct PushFull : public Rule {
    PushFull() : Rule("PushFull") {}
    bool precondition(const State& state) { return state.stack.isFull(); }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.stack.push(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

struct PushOK : public Rule {
    PushOK() : Rule("PushOK") {}
    bool precondition(const State& state) { return !state.stack.isFull(); }
    void action(State& state) {
        const U32 value = STest::Pick::any();
        const auto status = state.stack.push(value);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelStack.push_back(value);
    }
};

extern At at;

extern Clear clear;

extern Peek peek;

extern PopEmpty popEmpty;

extern PopOK popOK;

extern PushFull pushFull;

extern PushOK pushOK;

}  // namespace Rules

}  // namespace StackTest

}  // namespace Fw

#endif
