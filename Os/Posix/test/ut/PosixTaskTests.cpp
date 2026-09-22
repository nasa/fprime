// ======================================================================
// \title Os/Posix/test/ut/PosixTaskTests.cpp
// \brief tests for posix implementation for Os::Task
// ======================================================================
#include <gtest/gtest.h>
#include <pthread.h>
#include <sched.h>
#include <atomic>
#include "Fw/Types/String.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/Task.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace {
//! Scheduling information observed from within a running task
struct ObservedSchedule {
    std::atomic<int> policy{-1};
    std::atomic<int> priority{-1};
    std::atomic<bool> done{false};
};

//! Task routine that records the scheduling policy and priority of the calling thread
void recordScheduleRoutine(void* pointer) {
    ObservedSchedule& observed = *static_cast<ObservedSchedule*>(pointer);
    int policy = -1;
    sched_param param;
    param.sched_priority = -1;
    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        observed.policy = policy;
        observed.priority = param.sched_priority;
    }
    observed.done = true;
}

//! Start a task with the supplied priority and run it to completion; the task records its own scheduling
void runAndObserve(const FwTaskPriorityType priority, ObservedSchedule& observed) {
    Os::Task task;
    Fw::String name("SchedTask");
    Os::Task::Arguments arguments(name, recordScheduleRoutine, &observed, priority);
    ASSERT_EQ(task.start(arguments), Os::Task::Status::OP_OK) << "Task failed to start";
    ASSERT_EQ(task.join(), Os::Task::Status::OP_OK) << "Task failed to join";
    ASSERT_TRUE(observed.done);
    ASSERT_EQ(task.getPriority(), priority);
}

//! Fixture isolating these tests from the task registry left behind by the common task tests
class PosixTaskScheduling : public ::testing::Test {
  protected:
    void SetUp() override { Os::Task::registerTaskRegistry(nullptr); }
};
}  // namespace

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// The FPP-defined sentinel and the C++ constant must agree, and must not alias other sentinels or SCHED_RR priorities
TEST_F(PosixTaskScheduling, NonRealtimeSentinelValue) {
    // Copy to locals: gtest binds by reference, which would ODR-use the constexpr members in C++14
    const FwTaskPriorityType sentinel = Os::Posix::Task::PosixTask::TASK_PRIORITY_NON_REALTIME;
    const FwTaskPriorityType fpp_sentinel = static_cast<FwTaskPriorityType>(Os::Posix::TASK_PRIORITY_NON_REALTIME);
    const FwTaskPriorityType default_priority = Os::Task::TASK_PRIORITY_DEFAULT;
    ASSERT_EQ(sentinel, fpp_sentinel);
    ASSERT_NE(sentinel, default_priority);
    const int min_rr = sched_get_priority_min(SCHED_RR);
    const int max_rr = sched_get_priority_max(SCHED_RR);
    ASSERT_TRUE(static_cast<int>(sentinel) < min_rr || static_cast<int>(sentinel) > max_rr)
        << "Sentinel collides with a valid SCHED_RR priority";
}

// A task started with TASK_PRIORITY_NON_REALTIME runs under SCHED_OTHER, regardless of permissions
TEST_F(PosixTaskScheduling, NonRealtimeTaskUsesSchedOther) {
    ObservedSchedule observed;
    runAndObserve(Os::Posix::Task::PosixTask::TASK_PRIORITY_NON_REALTIME, observed);
    ASSERT_EQ(observed.policy.load(), SCHED_OTHER);
}

// When this thread can itself run under SCHED_RR, an inheriting task would also be SCHED_RR; the sentinel must
// still yield SCHED_OTHER. Skipped when realtime scheduling is not permitted (the common CI case).
TEST_F(PosixTaskScheduling, NonRealtimeTaskUsesSchedOtherFromRealtimeCaller) {
    int original_policy = -1;
    sched_param original_param;
    original_param.sched_priority = -1;
    ASSERT_EQ(pthread_getschedparam(pthread_self(), &original_policy, &original_param), 0);

    sched_param realtime_param;
    realtime_param.sched_priority = sched_get_priority_min(SCHED_RR);
    if (pthread_setschedparam(pthread_self(), SCHED_RR, &realtime_param) != 0) {
        GTEST_SKIP() << "Insufficient permission to run the caller under SCHED_RR";
    }

    ObservedSchedule inherited;
    runAndObserve(Os::Task::TASK_PRIORITY_DEFAULT, inherited);
    ObservedSchedule observed;
    runAndObserve(Os::Posix::Task::PosixTask::TASK_PRIORITY_NON_REALTIME, observed);
    // Restore before asserting so a failure does not leave the test thread realtime
    EXPECT_EQ(pthread_setschedparam(pthread_self(), original_policy, &original_param), 0);

    ASSERT_EQ(inherited.policy.load(), SCHED_RR);
    ASSERT_EQ(observed.policy.load(), SCHED_OTHER);
}

// A task started with TASK_PRIORITY_DEFAULT inherits the (non-realtime) scheduling of this test process
TEST_F(PosixTaskScheduling, DefaultPriorityTaskInheritsScheduling) {
    int expected_policy = -1;
    sched_param expected_param;
    expected_param.sched_priority = -1;
    ASSERT_EQ(pthread_getschedparam(pthread_self(), &expected_policy, &expected_param), 0);

    ObservedSchedule observed;
    runAndObserve(Os::Task::TASK_PRIORITY_DEFAULT, observed);
    ASSERT_EQ(observed.policy.load(), expected_policy);
    ASSERT_EQ(observed.priority.load(), expected_param.sched_priority);
}

// A task started with a numeric priority runs under SCHED_RR when permitted, otherwise falls back to inherited
// scheduling. Either way it must never be silently placed in SCHED_OTHER at a realtime priority.
TEST_F(PosixTaskScheduling, NumericPriorityTaskUsesSchedRrOrFallsBack) {
    const FwTaskPriorityType priority = static_cast<FwTaskPriorityType>(sched_get_priority_min(SCHED_RR));
    ObservedSchedule observed;
    runAndObserve(priority, observed);
    if (observed.policy.load() == SCHED_RR) {
        ASSERT_EQ(observed.priority.load(), static_cast<int>(priority));
    } else {
        int expected_policy = -1;
        sched_param expected_param;
        expected_param.sched_priority = -1;
        ASSERT_EQ(pthread_getschedparam(pthread_self(), &expected_policy, &expected_param), 0);
        ASSERT_EQ(observed.policy.load(), expected_policy);
        ASSERT_EQ(observed.priority.load(), expected_param.sched_priority);
    }
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
