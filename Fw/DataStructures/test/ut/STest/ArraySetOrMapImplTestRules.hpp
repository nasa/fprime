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
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) < State::capacity; }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.impl.getSize();
        const auto expectedSize = state.modelMapContains(key) ? size : size + 1;
        const auto status = state.impl.insert(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(state.impl.getSize(), expectedSize);
        state.modelMap[key] = value;
    }
};

extern InsertNotFull insertNotFull;

struct InsertFull : public Rule {
    InsertFull() : Rule("InsertFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.impl.getSize();
        const auto expectedStatus = state.modelMapContains(key) ? Success::SUCCESS : Success::FAILURE;
        const auto status = state.impl.insert(key, value);
        ASSERT_EQ(status, expectedStatus);
        ASSERT_EQ(state.impl.getSize(), size);
    }
};

extern InsertFull insertFull;

struct At : public Rule {
    At() : Rule("At") {}
    bool precondition(const State& state) { return state.impl.getSize() > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto* it = state.impl.getHeadIterator();
        for (FwSizeType i = 0; i < size; i++) {
            const auto& it1 = state.impl.at(i);
            ASSERT_NE(it, nullptr);
            const auto& it2 = *it;
            ASSERT_EQ(it1.getKey(), it2.getKey());
            ASSERT_EQ(it1.getValue(), it2.getValue());
            it = it->getNextIterator();
        }
    }
};

extern At at;

struct RemoveExisting : public Rule {
    RemoveExisting() : Rule("RemoveExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        const auto& it = state.impl.at(index);
        const auto key = it.getKey();
        const auto expectedValue = it.getValue();
        State::ValueType value = 0;
        const auto status = state.impl.remove(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
        const auto n = state.modelMap.erase(key);
        ASSERT_EQ(n, 1);
        ASSERT_EQ(state.impl.getSize(), state.modelMap.size());
    }
};

extern RemoveExisting removeExisting;

#if 0
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
