

#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"
#include <sys/time.h>


void
wait_for_state_with_timeout(Os::Test::Task::TestTaskInfo &info, const Os::Test::Task::TestTaskInfo::Lifecycle &stage,
                            const FwSizeType delay_ms) {
    // Loop waiting for transition in preparation for join (with timeout)
    FwSizeType i = 0;
    for (i = 0; i < (delay_ms * 100); i++) {
        Os::Task::delay(Fw::Time(0, 10));
        if (info.stage() == stage) {
            break;
        }
        // Run cooperative multitasking when necessary
        if (info.m_task.isCooperative()) {
            // TODO: run
        }
    }
    ASSERT_LT(i, (delay_ms * 100)) << "Task did not run within " << delay_ms << "ms";
    ASSERT_EQ(info.stage(), stage) << "Task lifecycle inconsistent";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Start
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::Start::Start() :
        STest::Rule<Os::Test::Task::Tester>("Start") {
}


bool Os::Test::Task::Tester::Start::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return state.m_tasks.size() < Os::Test::Task::Tester::MAX_THREAD_COUNT;
}


void Os::Test::Task::Tester::Start::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Start \n");

    std::shared_ptr<TestTaskInfo> new_task = std::make_shared<TestTaskInfo>();
    state.m_tasks.push_back(new_task);


    Fw::String name("StartRuleTask");
    new_task->start(&TestTaskInfo::standard_task);
    // Ensure it starts in the correct state
    ASSERT_EQ(new_task->stage(), TestTaskInfo::Lifecycle::BEGINNING);

    // Poke the task into the MIDDLE state
    new_task->signal();
    wait_for_state_with_timeout(*new_task, TestTaskInfo::MIDDLE, 100);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Join
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::Join::Join() :
        STest::Rule<Os::Test::Task::Tester>("Join") {
}

bool Os::Test::Task::Tester::Join::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return not state.m_tasks.empty();
}


void Os::Test::Task::Tester::Join::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Join\n");
    TestTaskInfo joiner_task;
    const U32 random_index = STest::Pick::lowerUpper(0, state.m_tasks.size() - 1);

    // Pop the middle
    std::shared_ptr<TestTaskInfo> other_task = state.m_tasks[random_index];
    state.m_tasks.erase(state.m_tasks.begin() + random_index);

    // Set the other argument
    joiner_task.m_other = other_task;

    joiner_task.start(&TestTaskInfo::joining_task);

    // Ensure it starts in the correct state for joining
    ASSERT_EQ(other_task->stage(), TestTaskInfo::Lifecycle::MIDDLE);

    // Poke the task into the MIDDLE state
    joiner_task.signal();
    wait_for_state_with_timeout(joiner_task, TestTaskInfo::MIDDLE, 100);

    // Make the other task move from MIDDLE state to end state and wait for JOINED state
    other_task->signal();
    wait_for_state_with_timeout(joiner_task, TestTaskInfo::END, 100);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CheckState
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::CheckState::CheckState() :
        STest::Rule<Os::Test::Task::Tester>("CheckState") {
}


bool Os::Test::Task::Tester::CheckState::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return true;
}


void Os::Test::Task::Tester::CheckState::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Check State\n");
    std::shared_ptr<TestTaskInfo> task;
    const U32 random_index = STest::Pick::lowerUpper(0, state.m_tasks.size());

    // Allow small chance to check new task is in NOT_STARTED state
    if (random_index >= state.m_tasks.size()) {
        task = std::make_shared<TestTaskInfo>();
    } else {
        task = state.m_tasks[random_index];
    }
    ASSERT_EQ(task->m_task.getState(), task->m_state) << "Task state progression not as expected.";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Delay
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::Delay::Delay() :
        STest::Rule<Os::Test::Task::Tester>("Delay") {
}

bool Os::Test::Task::Tester::Delay::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return true;
}

void Os::Test::Task::Tester::Delay::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Delay\n");
    const U32 delay_micro_seconds = STest::Pick::lowerUpper(0, MAX_DELAY_MICRO_SECONDS);
    Fw::Time delay(delay_micro_seconds / 1000000, delay_micro_seconds % 1000000);

    timeval start;
    timeval end;
    ASSERT_EQ(gettimeofday(&start, nullptr), 0) << "Failed to get time";
    Os::Task::delay(delay);
    ASSERT_EQ(gettimeofday(&end, nullptr), 0) << "Failed to get time";

    U32 wall_clock_delay_micro = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    ASSERT_GE(wall_clock_delay_micro, delay_micro_seconds);
    ASSERT_LT(wall_clock_delay_micro, delay_micro_seconds + 100000) << "Delay not within 100ms";

}

// ------------------------------------------------------------------------------------------------------
// Rule:  CheckTaskCount
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::CheckTaskCount::CheckTaskCount() :
        STest::Rule<Os::Test::Task::Tester>("CheckTaskCount") {
}


bool Os::Test::Task::Tester::CheckTaskCount::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return true;
}


void Os::Test::Task::Tester::CheckTaskCount::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: CheckTaskCount \n");
    FwSizeType count = TestTaskInfo::s_task_count;
    ASSERT_EQ(Os::Task::getNumTasks(), count) << "Task count miss-match";
}

