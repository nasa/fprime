// ======================================================================
// \title Os/test/ut/task/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Task.hpp"
#include "Os/test/ut/task/CommonTests.hpp"
#include "Os/test/ut/task/RulesHeaders.hpp"
#include "Fw/Types/String.hpp"

static constexpr U32 RANDOM_BOUND = 1000;

namespace Os {
namespace Test {
namespace Task {
    FwSizeType TestTaskInfo::s_task_count = 0;

    Tester::~Tester() {
        this->m_tasks.erase(this->m_tasks.begin(), this->m_tasks.end());
    }

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
        this->m_lock.lock();
        this->m_signal = true;
        this->m_lock.unlock();
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
            // Run cooperative multitasking when necessary
            Tester::getCurrentRegistry()->invokeRoutines();
        }
        this->m_task.join();
    }

    void TestTaskInfo::standard_task(void *argument) {
        ASSERT_NE(argument, nullptr) << "Test provided bad argument pointer";
        TestTaskInfo& task_info = *reinterpret_cast<TestTaskInfo*>(argument);
        // On the first iteration set start-up items
        if (task_info.stage() == TestTaskInfo::Lifecycle::UNSET) {
            task_info.m_lock.lock();
            task_info.m_stage = Lifecycle::BEGINNING;
            task_info.m_state = Os::Task::RUNNING;
            task_info.m_lock.unlock();
        }
        // Loop when the thread is not cooperative and lifecycle is not over
        do {
            task_info.step();
        } while ((not task_info.m_task.isCooperative()) && (task_info.stage() != TestTaskInfo::Lifecycle::END));
        // Update if in end stage
        if (task_info.stage() == TestTaskInfo::Lifecycle::END) {
            task_info.m_state = Os::Task::State::EXITED;
        }
    }

    void TestTaskInfo::joining_task(void *argument) {
        ASSERT_NE(argument, nullptr) << "Test provided bad argument pointer";
        TestTaskInfo& task_info = *reinterpret_cast<TestTaskInfo*>(argument);
        ASSERT_NE(task_info.m_other, nullptr) << "Other pointer not properly set";
        // On the first iteration set start-up items
        if (task_info.stage() == TestTaskInfo::Lifecycle::UNSET) {
            task_info.m_lock.lock();
            task_info.m_stage = Lifecycle::BEGINNING;
            task_info.m_state = Os::Task::RUNNING;
            task_info.m_lock.unlock();
        }

        do {
            // Signal will come from test thread then this will join
            if (task_info.stage() == TestTaskInfo::Lifecycle::MIDDLE) {
                task_info.m_other->m_task.join();
                task_info.m_lock.lock();
                task_info.m_stage = TestTaskInfo::Lifecycle::END;
                task_info.m_lock.unlock();
                break;
            }
            task_info.step();
        } while (not task_info.m_task.isCooperative());
    }

    Tester* Tester::s_current_registry = nullptr;

    Tester::Tester() {
        Tester::s_current_registry = this;
        Os::Task::registerTaskRegistry(this);
    }

    Tester* Tester::getCurrentRegistry() {
        EXPECT_NE(s_current_registry, nullptr) << "Registry not properly initialized";
        return s_current_registry;
    }

    void Tester::invokeRoutines() {
        for (Os::Task* task : this->m_all_tasks) {
            if (task->isCooperative()) {
                task->invokeRoutine();
            }
        }
    }

    void Tester::addTask(Os::Task *task) {
        this->m_last_task = task;
        this->m_all_tasks.push_back(task);
        ASSERT_GT(this->m_all_tasks.size(), 0) << "Where have all the tasks gone?";
    }

    void Tester::removeTask(Os::Task *task) {
        ASSERT_GT(this->m_all_tasks.size(), 0) << "Where have all the tasks gone?";
        this->m_last_task = nullptr;
        size_t i = 0;
        for (i = 0; i < this->m_all_tasks.size(); i++) {
            if (task == this->m_all_tasks[i]) {
                break;
            }
        }
        ASSERT_LT(i, this->m_all_tasks.size()) << "Task not found";
        this->m_all_tasks.erase(this->m_all_tasks.begin() + i);
    }
}
}
}

// Ensure that a task can start
TEST(Functionality, StartTask) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start rule;
    rule.apply(tester);
}

// Ensure that a started task can be joined to
TEST(Functionality, StartJoinTask) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    start_rule.apply(tester);
    join_rule.apply(tester);

}

// Ensure that state transitions work reliably
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

// Ensure that delay works as expected
TEST(Functionality, Delay) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Delay rule;
    rule.apply(tester);
}

// Ensure task counting works
TEST(Functionality, CheckTaskCount) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::CheckTaskCount check_rule;
    check_rule.apply(tester);
    start_rule.apply(tester);
    check_rule.apply(tester);
}

// Ensure that a join on an unstarted thread returns a bad status
TEST(Functionality, JoinInvalidState) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::JoinInvalidState rule;
    rule.apply(tester);
}

// Ensure that a start with a nullptr causes an assertion failure
TEST(Functionality, StartIllegalRoutine) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::StartIllegalRoutine rule;
    rule.apply(tester);
}

// Randomized testing for the win
TEST(Functionality, RandomizedTesting) {
    Os::Test::Task::Tester tester;
    // Enumerate all rules and construct an instance of each
    Os::Test::Task::Tester::Start start_rule;
    Os::Test::Task::Tester::Join join_rule;
    Os::Test::Task::Tester::CheckState check_state_rule;
    Os::Test::Task::Tester::CheckTaskCount check_count_rule;
    Os::Test::Task::Tester::JoinInvalidState invalid_join_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::Task::Tester>* rules[] = {
            &start_rule,
            &join_rule,
            &check_state_rule,
            &check_count_rule,
            &invalid_join_rule
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
