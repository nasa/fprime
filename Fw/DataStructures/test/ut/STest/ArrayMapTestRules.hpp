// ======================================================================
// \title  ArrayMapTestRules.hpp
// \author bocchino
// \brief  hpp file for ArrayMap test rules
// ======================================================================

#ifndef ArrayMapTestRules_HPP
#define ArrayMapTestRules_HPP

#include <gtest/gtest.h>

#include "Fw/DataStructures/test/ut/STest/ArrayMapTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace ArrayMapTest {

using Rule = STest::Rule<State>;

namespace Rules {

#if 0
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

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.impl.getSize() > 0; }
    void action(State& state) {
        state.impl.clear();
        ASSERT_EQ(state.impl.getSize(), 0);
        state.modelMap.clear();
    }
};

struct FindExisting : public Rule {
    FindExisting() : Rule("FindExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        const auto& it = state.impl.at(index);
        const auto key = it.getKey();
        const auto expectedValue = state.modelMap[key];
        State::ValueType value = 0;
        const auto status = state.impl.find(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
    }
};

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

struct Remove : public Rule {
    Remove() : Rule("Remove") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        ASSERT_EQ(size, state.modelMap.size());
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.impl.remove(key, value);
        if (state.modelMap.count(key) != 0) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelMap[key]);
            ASSERT_EQ(state.impl.getSize(), size - 1);
        }
        else {
            ASSERT_EQ(status, Success::FAILURE);
            ASSERT_EQ(state.impl.getSize(), size);
        }
        (void) state.modelMap.erase(key);
        ASSERT_EQ(state.impl.getSize(), state.modelMap.size());
    }
};

struct RemoveExisting : public Rule {
    RemoveExisting() : Rule("RemoveExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        const auto& it = state.impl.at(index);
        const auto key = it.getKey();
        const auto expectedValue = state.modelMap[key];
        State::ValueType value = 0;
        const auto status = state.impl.remove(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
        const auto n = state.modelMap.erase(key);
        ASSERT_EQ(n, 1);
        ASSERT_EQ(state.impl.getSize(), state.modelMap.size());
    }
};

extern At at;

extern Clear clear;

extern FindExisting findExisting;

extern InsertFull insertFull;

extern InsertNotFull insertNotFull;

extern Remove remove;

extern RemoveExisting removeExisting;
#endif

};  // namespace Rules

}  // namespace ArrayMapTest

}  // namespace Fw

#endif
