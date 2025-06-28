// ======================================================================
// \title  ArraySetOrMapImplTestRules.hpp
// \author bocchino
// \brief  hpp file for ArraySetOrMapImpl test rules
// ======================================================================

#ifndef ArraySetOrMapImplTestRules_HPP
#define ArraySetOrMapImplTestRules_HPP

#include <gtest/gtest.h>

#include "Fw/DataStructures/test/ut/STest/ArraySetOrMapImplTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace ArraySetOrMapImplTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct InsertNotFull : public Rule {
    InsertNotFull() : Rule("InsertNotFull") {}
    bool precondition(const State& state) { 
      return static_cast<FwSizeType>(state.impl.getSize()) < State::capacity;
    }
    void action(State& state) {
      const State::KeyType key = state.getRandomKey();
      const State::ValueType value = state.getRandomValue();
      const auto status = state.impl.insert(key, value);
      ASSERT_EQ(status, Success::SUCCESS);
      state.modelMap[key] = value;
    }
};

extern InsertNotFull insertNotFull;

#if 0
struct PushFull : public Rule {
    PushFull() : Rule("PushFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto item = State::getRandomItem();
        const auto status = state.impl.push(item);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

extern PushFull pushFull;

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return state.impl.getSize() > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        ASSERT_EQ(state.impl.at(index), state.modelArraySetOrMapImpl.at(size - 1 - index));
    }
};

extern At at;

struct PopOK : public Rule {
    PopOK() : Rule("PopOK") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        U32 value = 0;
        const auto status = state.impl.pop(value);
        ASSERT_EQ(status, Success::SUCCESS);
        const auto expectedValue = state.modelArraySetOrMapImpl.at(size - 1);
        ASSERT_EQ(value, expectedValue);
        state.modelArraySetOrMapImpl.pop_back();
        ASSERT_EQ(state.impl.getSize(), state.modelArraySetOrMapImpl.size());
    }
};

extern PopOK popOK;

struct PopEmpty : public Rule {
    PopEmpty() : Rule("PopEmpty") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) == 0; }
    void action(State& state) {
        U32 value = 0;
        const auto status = state.impl.pop(value);
        ASSERT_EQ(status, Success::FAILURE);
    }
};

extern PopEmpty popEmpty;

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.impl.getSize() > 0; }
    void action(State& state) {
        state.impl.clear();
        ASSERT_EQ(state.impl.getSize(), 0);
        state.modelArraySetOrMapImpl.clear();
    }
};

extern Clear clear;

#endif
};  // namespace Rules

}  // namespace ArraySetOrMapImplTest

}  // namespace Fw

#endif
