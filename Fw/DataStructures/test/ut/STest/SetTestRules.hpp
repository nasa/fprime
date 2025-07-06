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
        const auto e = state.getElement();
        const auto status = state.set.find(e);
        const auto expectedStatus = state.modelSetContains(e) ? Success::SUCCESS : Success::FAILURE;
        ASSERT_EQ(status, expectedStatus);
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
            const auto e = it->getElement();
            const auto status = state.set.find(e);
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_TRUE(state.modelSetContains(e));
            it = it->getNextSetIterator();
        }
    }
};

struct InsertFull : public Rule {
    InsertFull() : Rule("InsertFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) >= State::capacity; }
    void action(State& state) {
        const auto e = state.getElement();
        const auto size = state.set.getSize();
        const auto expectedStatus = state.modelSetContains(e) ? Success::SUCCESS : Success::FAILURE;
        const auto status = state.set.insert(e);
        ASSERT_EQ(status, expectedStatus);
        ASSERT_EQ(state.set.getSize(), size);
    }
};

struct InsertNotFull : public Rule {
    InsertNotFull() : Rule("InsertNotFull") {}
    bool precondition(const State& state) { return static_cast<FwSizeType>(state.set.getSize()) < State::capacity; }
    void action(State& state) {
        const auto e = state.getElement();
        const auto size = state.set.getSize();
        const auto expectedSize = state.modelSetContains(e) ? size : size + 1;
        const auto status = state.set.insert(e);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(state.set.getSize(), expectedSize);
        state.modelSet.insert(e);
    }
};

#if 0
struct Remove : public Rule {
    Remove() : Rule("Remove") {}
    bool precondition(const State& state) { return true; }
    void action(State& state) {
        const auto size = state.set.getSize();
        ASSERT_EQ(size, state.modelSet.size());
        const auto e = state.getElement();
        State::ValueType value = 0;
        const auto status = state.set.remove(e, value);
        if (state.modelSetContains(e)) {
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(value, state.modelSet[e]);
            ASSERT_EQ(state.set.getSize(), size - 1);
        }
        else {
            ASSERT_EQ(status, Success::FAILURE);
            ASSERT_EQ(state.set.getSize(), size);
        }
        (void) state.modelSet.erase(e);
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
        const auto e = it->getElement();
        const auto expectedValue = state.modelSet[e];
        State::ValueType value = 0;
        const auto status = state.set.remove(e, value);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(value, expectedValue);
        const auto n = state.modelSet.erase(e);
        ASSERT_EQ(n, 1);
        ASSERT_EQ(state.set.getSize(), state.modelSet.size());
    }
};
#endif

extern Clear clear;

extern Find find;

extern FindExisting findExisting;

extern InsertFull insertFull;

extern InsertNotFull insertNotFull;

#if 0
extern Remove remove;

extern RemoveExisting removeExisting;
#endif

};  // namespace Rules

}  // namespace SetTest

}  // namespace Fw

#endif
