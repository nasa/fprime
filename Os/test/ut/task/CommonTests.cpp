// ======================================================================
// \title Os/test/ut/task/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Task.hpp"
#include "Os/test/ut/task/CommonTests.hpp"
#include "Os/test/ut/task/RulesHeaders.hpp"
#include "Fw/Types/String.hpp"

static const U32 RANDOM_BOUND = 1000;

namespace Os {
namespace Test {
namespace Task {
    FwSizeType TestTaskInfo::s_task_count = 0;

    TestTaskInfo::~TestTaskInfo() {
        this->stop();
    }

    void TestTaskInfo::step() {
        this->m_lock.lock();
        // Read update and clear signal
        if (this->m_signal) {
            this->m_stage = (this->m_stage == Lifecycle::END) ? Lifecycle::END : static_cast<Lifecycle>(this->m_stage + 1);
        }
        this->m_signal = false;
        this->m_lock.unlock();
    }

    void TestTaskInfo::signal() {
        // Block waiting for m_signal to be false
        bool loop = true;
        while (loop) {
            this->m_lock.lock();
            // When signal is low, stop looping and make signal high
            if (not this->m_signal) {
                loop = false;
                this->m_signal = true;
            }
            this->m_lock.unlock();
        }
    }

    TestTaskInfo::Lifecycle TestTaskInfo::stage() {
        Lifecycle stage = Lifecycle::UNSET;
        this->m_lock.lock();
        stage = this->m_stage;
        this->m_lock.unlock();
        return stage;
    }

    void TestTaskInfo::start(Os::Task::taskRoutine routine) {
        Fw::String name("TaskName");
        Os::Task::Arguments arguments(name, routine, this);
        s_task_count += 1;
        this->m_task.start(arguments);
    }

    void TestTaskInfo::stop() {
        this->m_lock.lock();
        Os::Task::State state = this->m_state;
        this->m_lock.unlock();
        // Not started task, break out
        if (state == Os::Task::State::NOT_STARTED) {
            return;
        }
        while (this->stage() != Lifecycle::END) {
            this->signal();
        }
        this->m_task.join();
    }

    void TestTaskInfo::standard_task(void *argument) {
        ASSERT_NE(argument, nullptr) << "Test provided bad argument pointer";
        TestTaskInfo& task_info = *reinterpret_cast<TestTaskInfo*>(argument);
        task_info.m_lock.lock();
        task_info.m_state = Os::Task::RUNNING;
        task_info.m_lock.unlock();
        // Loop until the lifecycle is at the end
        while (task_info.stage() != TestTaskInfo::Lifecycle::END) {
            task_info.step();
        }
        task_info.m_state = Os::Task::State::EXITED;
    }

    void TestTaskInfo::joining_task(void *argument) {
        ASSERT_NE(argument, nullptr) << "Test provided bad argument pointer";
        TestTaskInfo& task_info = *reinterpret_cast<TestTaskInfo*>(argument);
        ASSERT_NE(task_info.m_other, nullptr) << "Other pointer not properly set";

        // Loop until the lifecycle is at the MIDDLE
        while (task_info.stage() != TestTaskInfo::Lifecycle::MIDDLE) {
            task_info.step();
        }
        // Signal will come from test thread then this will join
        task_info.m_other->m_task.join();
        task_info.m_lock.lock();
        task_info.m_stage = TestTaskInfo::Lifecycle::END;
        task_info.m_lock.unlock();
    }
}
}
}

// Ensure that open mode changes work reliably
TEST(Functionality, StartTask) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start rule;
    rule.apply(tester);
}

// Ensure that open mode changes work reliably
TEST(Functionality, StartJoinTask) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    start_rule.apply(tester);
    join_rule.apply(tester);

}

// Ensure that open mode changes work reliably
TEST(Functionality, CheckState) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::CheckState check_rule;
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    check_rule.apply(tester);
    start_rule.apply(tester);
    check_rule.apply(tester);
    join_rule.apply(tester);
    check_rule.apply(tester);
}

// Ensure that open mode changes work reliably
TEST(Functionality, Delay) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Delay rule;
    rule.apply(tester);

}

// Ensure that open mode changes work reliably
TEST(Functionality, CheckTaskCount) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    Os::Test::Task::Tester::CheckTaskCount check_rule;
    check_rule.apply(tester);
    start_rule.apply(tester);
    check_rule.apply(tester);
    join_rule.apply(tester);
    check_rule.apply(tester);
}

// Ensure a write followed by a read produces valid data
TEST(Functionality, RandomizedTesting) {
    Os::Test::Task::Tester tester;
    // Enumerate all rules and construct an instance of each
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    Os::Test::Task::Tester::CheckState check_state_rule;
    Os::Test::Task::Tester::CheckTaskCount check_count_rule;


    // Place these rules into a list of rules
    STest::Rule<Os::Test::Task::Tester>* rules[] = {
            &start_rule,
            &join_rule,
            &check_state_rule,
            &check_count_rule
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::Task::Tester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::Task::Tester> bounded(
            "Bounded Random Rules Scenario",
            random,
            RANDOM_BOUND
    );
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}
