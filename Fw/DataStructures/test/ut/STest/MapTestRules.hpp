// ======================================================================
// \title  MapTestRules.hpp
// \author bocchino
// \brief  hpp file for map test rules
// ======================================================================

#ifndef MapTestRules_HPP
#define MapTestRules_HPP

#include "Fw/DataStructures/test/ut/STest/MapTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace MapTest {

using Rule = STest::Rule<State>;

namespace Rules {

struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return !state.map.isEmpty(); }
    void action(State& state) {
        state.map.clear();
        ASSERT_EQ(state.map.getSize(), 0);
        state.modelMap.clear();
    }
};

struct Find : public Rule {
    Find() : Rule("Find") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.map.find(key, value);
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
    bool precondition(const State& state) { return !state.map.isEmpty(); }
    void action(State& state) {
        for (auto& entry : state.map) {
            const auto key = entry.getKey();
            const auto expectedValue = state.modelMap[key];
            State::ValueType value = 0;
            const auto status = state.map.find(key, value);
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, expectedValue);
        }
    }
};

struct InsertExisting : public Rule {
    InsertExisting() : Rule("InsertExisting") {}
    bool precondition(const State& state) { return !state.map.isEmpty(); }
    void action(State& state) {
        const auto size = state.map.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        auto it = state.map.begin();
        for (FwSizeType i = 0; i < index; i++) {
            ASSERT_TRUE(it.isInRange());
            it++;
        }
        ASSERT_TRUE(it.isInRange());
        const auto key = it->getKey();
        const auto value = state.getValue();
        const auto status = state.map.insert(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        state.modelMap[key] = value;
        ASSERT_EQ(state.map.getSize(), size);
    }
};

struct InsertFull : public Rule {
    InsertFull() : Rule("InsertFull") {}
    bool precondition(const State& state) { return state.map.isFull(); }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.map.getSize();
        const auto expectedStatus = state.modelMapContains(key) ? Success::SUCCESS : Success::FAILURE;
        const auto status = state.map.insert(key, value);
        ASSERT_EQ(status, expectedStatus);
        ASSERT_EQ(state.map.getSize(), size);
    }
};

struct InsertNotFull : public Rule {
    InsertNotFull() : Rule("InsertNotFull") {}
    bool precondition(const State& state) { return !state.map.isFull(); }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.map.getSize();
        const auto expectedSize = state.modelMapContains(key) ? size : size + 1;
        const auto status = state.map.insert(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(state.map.getSize(), expectedSize);
        state.modelMap[key] = value;
    }
};

struct Remove : public Rule {
    Remove() : Rule("Remove") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto size = state.map.getSize();
        ASSERT_EQ(size, state.modelMap.size());
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.map.remove(key, value);
        if (state.modelMapContains(key)) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelMap[key]);
            ASSERT_EQ(state.map.getSize(), size - 1);
        } else {
            ASSERT_EQ(status, Success::FAILURE);
            ASSERT_EQ(state.map.getSize(), size);
        }
        (void)state.modelMap.erase(key);
        ASSERT_EQ(state.map.getSize(), state.modelMap.size());
    }
};

struct RemoveExisting : public Rule {
    RemoveExisting() : Rule("RemoveExisting") {}
    bool precondition(const State& state) { return !state.map.isEmpty(); }
    void action(State& state) {
        const auto size = state.map.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        auto it = state.map.begin();
        for (FwSizeType i = 0; i < index; i++) {
            ASSERT_TRUE(it.isInRange());
            it++;
        }
        ASSERT_TRUE(it.isInRange());
        const auto key = (*it).getKey();
        const auto expectedValue = state.modelMap[key];
        State::ValueType value = 0;
        const auto status = state.map.remove(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
        const auto n = state.modelMap.erase(key);
        ASSERT_EQ(n, 1);
        ASSERT_EQ(state.map.getSize(), state.modelMap.size());
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

}  // namespace MapTest

}  // namespace Fw

#endif
