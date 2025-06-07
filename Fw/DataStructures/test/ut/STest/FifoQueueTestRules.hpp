#ifndef FifoQueueTestRules_HPP
#define FifoQueueTestRules_HPP

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

namespace FifoQueueTest {

using Rule = STest::Rule<State>;

struct Rules {

    struct EnqueueOK : public Rule {
        EnqueueOK() : Rule("EnqueueOK") {}
        bool precondition(const State& state) {
            return static_cast<FwSizeType>(state.modelQueue.size()) < State::capacity;
        }
        void action(State& state) {
            const U32 value = STest::Pick::any();
            const auto status = state.queue.enqueue(value);
            state.modelQueue.push_back(value);
            ASSERT_EQ(status, Success::SUCCESS);
        }
    };

    struct EnqueueFull : public Rule {
        EnqueueFull() : Rule("EnqueueFull") {}
        bool precondition(const State& state) {
            return static_cast<FwSizeType>(state.modelQueue.size()) >= State::capacity;
        }
        void action(State& state) {
            const auto item = State::getRandomItem();
            const auto status = state.queue.enqueue(item);
            ASSERT_EQ(status, Success::FAILURE);
        }
    };

    struct Clear : public Rule {
        Clear() : Rule("Clear") {}
        bool precondition(const State& state) { return true; }
        void action(State& state) {
            state.queue.clear();
            ASSERT_EQ(state.queue.getSize(), 0);
            state.modelQueue.clear();
        }
    };
};

}

}  // namespace Fw

#endif
