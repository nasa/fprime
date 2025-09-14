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

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.impl.getSize() > 0; }
    void action(State& state) {
        state.impl.clear();
        ASSERT_EQ(state.impl.getSize(), 0);
        state.modelMap.clear();
    }
};

struct Find : public Rule {
    Find() : Rule("Find") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.impl.find(key, value);
        if (state.modelMapContains(key)) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelMap[key]);
        } else {
            ASSERT_EQ(status, Success::FAILURE);
        }
    }
};

struct FindExisting : public Rule {
    FindExisting() : Rule("FindExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        auto it = state.impl.begin();
        for (FwSizeType i = 0; i < index; i++) {
            ASSERT_TRUE(it.isInRange());
            it.increment();
        }
        ASSERT_TRUE(it.isInRange());
        const auto key = it.getEntry().getKeyOrElement();
        const auto expectedValue = state.modelMap[key];
        State::ValueType value = 0;
        const auto status = state.impl.find(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
    }
};

struct InsertExisting : public Rule {
    InsertExisting() : Rule("InsertExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        auto it = state.impl.begin();
        for (FwSizeType i = 0; i < index; i++) {
            ASSERT_TRUE(it.isInRange());
            it.increment();
        }
        ASSERT_TRUE(it.isInRange());
        const auto key = it.getEntry().getKeyOrElement();
        const auto value = state.getValue();
        const auto status = state.impl.insert(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelMap[key] = value;
        ASSERT_EQ(state.impl.getSize(), size);
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
        } else {
            ASSERT_EQ(status, Success::FAILURE);
            ASSERT_EQ(state.impl.getSize(), size);
        }
        (void)state.modelMap.erase(key);
        ASSERT_EQ(state.impl.getSize(), state.modelMap.size());
    }
};

struct RemoveExisting : public Rule {
    RemoveExisting() : Rule("RemoveExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.impl.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.impl.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        auto it = state.impl.begin();
        for (FwSizeType i = 0; i < index; i++) {
            ASSERT_TRUE(it.isInRange());
            it.increment();
        }
        ASSERT_TRUE(it.isInRange());
        const auto key = it.getEntry().getKeyOrElement();
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

extern Clear clear;

extern Find find;

extern FindExisting findExisting;

extern InsertExisting insertExisting;

extern InsertFull insertFull;

extern InsertNotFull insertNotFull;

extern Remove remove;

extern RemoveExisting removeExisting;

}  // namespace Rules

}  // namespace ArraySetOrMapImplTest

}  // namespace Fw

#endif
