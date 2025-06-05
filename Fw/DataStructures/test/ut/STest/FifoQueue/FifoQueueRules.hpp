#ifndef FifoQueueRules_HPP
#define FifoQueueRules_HPP

#include "Fw/DataStructures/test/ut/STest/FifoQueue/FifoQueueTestState.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"

namespace Fw {

template <FwSizeType C>
struct FifoQueueRules {

    using TestState = FifoQueueTestState<U32, C>;

    using Rule = STest::Rule<TestState>;

    struct EnqueueOK : public Rule {
        EnqueueOK() : Rule("EnqueueOK") {}
        bool precondition(const TestState& state) { 
            return static_cast<FwSizeType>(state.abstractQueue.size()) < C;
        }
        void action(TestState& state) {
            const U32 value = STest::Pick::any();
            const auto status = state.queue.enqueue(value);
            state.abstractQueue.push_back(value);
            ASSERT_EQ(status, Success::SUCCESS);
        }
    };

    struct EnqueueFull : public Rule {
        EnqueueFull() : Rule("EnqueueFull") {}
        bool precondition(const TestState& state) { 
            return static_cast<FwSizeType>(state.abstractQueue.size()) >= C;
        }
        void action(TestState& state) {
            const U32 value = STest::Pick::any();
            const auto status = state.queue.enqueue(value);
            ASSERT_EQ(status, Success::FAILURE);
        }
    };

};

}  // namespace Fw

#endif
