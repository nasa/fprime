// ======================================================================
// \title  SetTestRules.hpp
// \author bocchino
// \brief  hpp file for set test rules
// ======================================================================

#ifndef SetTestRules_HPP
#define SetTestRules_HPP

#include "Fw/DataStructures/test/ut/STest/SetTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace SetTest {

using Rule = STest::Rule<State>;

namespace Rules {

#if 0
struct Clear : public Rule {
    Clear() : Rule("Clear") {}
    bool precondition(const State& state) { return state.set.getSize() > 0; }
    void action(State& state) {
        state.set.clear();
        ASSERT_EQ(state.set.getSize(), 0);
        state.modelSet.clear();
    }
};

struct Find : public Rule {
    Find() : Rule("Find") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.set.find(key, value);
        if (state.modelSetContains(key)) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelSet[key]);
        } else {
            ASSERT_EQ(status, Success::FAILURE);
        }
    }
};

struct FindExisting : public Rule {
    FindExisting() : Rule("FindExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.set.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        const auto* it = state.set.getHeadIterator();
        for (FwSizeType i = 0; i < index; i++) {
          ASSERT_NE(it, nullptr);
          it = it->getNextSetIterator();
        }
        ASSERT_NE(it, nullptr);
        const auto key = it->getKey();
        const auto expectedValue = state.modelSet[key];
        State::ValueType value = 0;
        const auto status = state.set.find(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
    }
};

struct InsertFull : public Rule {
    InsertFull() : Rule("InsertFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.set.getSize();
        const auto expectedStatus = state.modelSetContains(key) ? Success::SUCCESS : Success::FAILURE;
        const auto status = state.set.insert(key, value);
        ASSERT_EQ(status, expectedStatus);
        ASSERT_EQ(state.set.getSize(), size);
    }
};

struct InsertNotFull : public Rule {
    InsertNotFull() : Rule("InsertNotFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) < State::capacity; }
    void action(State& state) {
        const auto key = state.getKey();
        const auto value = state.getValue();
        const auto size = state.set.getSize();
        const auto expectedSize = state.modelSetContains(key) ? size : size + 1;
        const auto status = state.set.insert(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(state.set.getSize(), expectedSize);
        state.modelSet[key] = value;
    }
};

struct Remove : public Rule {
    Remove() : Rule("Remove") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto size = state.set.getSize();
        ASSERT_EQ(size, state.modelSet.size());
        const auto key = state.getKey();
        State::ValueType value = 0;
        const auto status = state.set.remove(key, value);
        if (state.modelSetContains(key)) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelSet[key]);
            ASSERT_EQ(state.set.getSize(), size - 1);
        }
        else {
            ASSERT_EQ(status, Success::FAILURE);
            ASSERT_EQ(state.set.getSize(), size);
        }
        (void) state.modelSet.erase(key);
        ASSERT_EQ(state.set.getSize(), state.modelSet.size());
    }
};

struct RemoveExisting : public Rule {
    RemoveExisting() : Rule("RemoveExisting") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) > 0; }
    void action(State& state) {
        const auto size = state.set.getSize();
        const auto index = STest::Pick::startLength(0, static_cast<U32>(size));
        const auto* it = state.set.getHeadIterator();
        for (FwSizeType i = 0; i < index; i++) {
          ASSERT_NE(it, nullptr);
          it = it->getNextSetIterator();
        }
        ASSERT_NE(it, nullptr);
        const auto key = it->getKey();
        const auto expectedValue = state.modelSet[key];
        State::ValueType value = 0;
        const auto status = state.set.remove(key, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
        const auto n = state.modelSet.erase(key);
        ASSERT_EQ(n, 1);
        ASSERT_EQ(state.set.getSize(), state.modelSet.size());
    }
};

extern Clear clear;

extern Find find;

extern FindExisting findExisting;

extern InsertFull insertFull;

extern InsertNotFull insertNotFull;

extern Remove remove;

extern RemoveExisting removeExisting;
#endif

};  // namespace Rules

}  // namespace SetTest

}  // namespace Fw

#endif
