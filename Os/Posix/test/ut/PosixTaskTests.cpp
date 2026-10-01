// ======================================================================
// \title Os/Posix/test/ut/PosixTaskTests.cpp
// \brief tests for posix implementation for Os::Task
// ======================================================================
#include <gtest/gtest.h>
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <climits>
#if defined(TGT_OS_TYPE_LINUX)
#include <sys/syscall.h>
#endif
#include "Fw/Types/String.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/Task.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace {
//! Nice value of the calling thread, or INT_MIN when unavailable (nice is per-thread on Linux only)
int currentNice() {
    int nice = INT_MIN;
#if defined(TGT_OS_TYPE_LINUX)
    errno = 0;
    const int value = getpriority(PRIO_PROCESS, static_cast<id_t>(syscall(SYS_gettid)));
    if ((value != -1) || (errno == 0)) {
        nice = value;
    }
#endif
    return nice;
}

//! A nice value above the inherited one, which an unprivileged thread may set, saturating at the maximum
int raisedNice(const int step) {
    const int inherited = currentNice();
    const int raised = inherited + step;
    return (raised > 19) ? 19 : raised;
}

//! Scheduling information observed from within a running task
struct ObservedSchedule {
    std::atomic<int> policy{-1};
    std::atomic<int> priority{-1};
    std::atomic<int> nice{INT_MIN};
    std::atomic<bool> done{false};
};

//! Record the scheduling policy, priority, and nice value of the calling thread
void observeSchedule(ObservedSchedule& observed) {
    int policy = -1;
    sched_param param;
    param.sched_priority = -1;
    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        observed.policy = policy;
        observed.priority = param.sched_priority;
    }
    observed.nice = currentNice();
    observed.done = true;
}

//! Task routine that records the scheduling of the calling thread
void recordScheduleRoutine(void* pointer) {
    observeSchedule(*static_cast<ObservedSchedule*>(pointer));
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
#ifdef __APPLE__
    // Darwin threads do not inherit the caller's SCHED_RR policy, so the inherited-vs-sentinel contrast cannot be shown
    GTEST_SKIP() << "Default-priority tasks do not inherit the caller's SCHED_RR policy on Darwin";
#endif
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

#if !(defined(POSIX_THREADS_USE_LINUX_PRIORITIES) && POSIX_THREADS_USE_LINUX_PRIORITIES)
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
#endif

// The Linux-defined priority bands resolve to the documented policy, priority and nice, with 99 and >139 clamped
TEST_F(PosixTaskScheduling, LinuxPriorityMappingBands) {
    using Os::Posix::Task::linux_priority_to_schedule;
    using Os::Posix::Task::LinuxSchedule;
    const CHAR* name = "MapTask";

    LinuxSchedule schedule = linux_priority_to_schedule(name, 0);
    EXPECT_EQ(schedule.m_policy, SCHED_RR);
    EXPECT_EQ(schedule.m_priority, 99);

    schedule = linux_priority_to_schedule(name, 50);
    EXPECT_EQ(schedule.m_policy, SCHED_RR);
    EXPECT_EQ(schedule.m_priority, 49);

    schedule = linux_priority_to_schedule(name, 98);
    EXPECT_EQ(schedule.m_policy, SCHED_RR);
    EXPECT_EQ(schedule.m_priority, 1);

    // 99 is unoccupied on Linux: clamped to the lowest realtime priority
    schedule = linux_priority_to_schedule(name, 99);
    EXPECT_EQ(schedule.m_policy, SCHED_RR);
    EXPECT_EQ(schedule.m_priority, 1);

    schedule = linux_priority_to_schedule(name, 100);
    EXPECT_EQ(schedule.m_policy, SCHED_OTHER);
    EXPECT_EQ(schedule.m_priority, 0);
    EXPECT_EQ(schedule.m_nice, -20);

    schedule = linux_priority_to_schedule(name, 120);
    EXPECT_EQ(schedule.m_policy, SCHED_OTHER);
    EXPECT_EQ(schedule.m_nice, 0);

    schedule = linux_priority_to_schedule(name, 139);
    EXPECT_EQ(schedule.m_policy, SCHED_OTHER);
    EXPECT_EQ(schedule.m_nice, 19);

    // Beyond the Linux range: clamped to the lowest priority
    schedule = linux_priority_to_schedule(name, 140);
    EXPECT_EQ(schedule.m_policy, SCHED_OTHER);
    EXPECT_EQ(schedule.m_nice, 19);

    schedule = linux_priority_to_schedule(name, 253);
    EXPECT_EQ(schedule.m_policy, SCHED_OTHER);
    EXPECT_EQ(schedule.m_nice, 19);
}

// The Linux priority definition is fixed by the kernel; confirm the platform agrees with the constants used
TEST_F(PosixTaskScheduling, LinuxPriorityDefinitionMatchesPlatform) {
#if !defined(TGT_OS_TYPE_LINUX)
    GTEST_SKIP() << "Linux-defined priorities describe the Linux scheduler only";
#endif
    const int realtime_base = Os::Posix::Task::LINUX_PRIORITY_REALTIME_BASE;
    const int realtime_max = Os::Posix::Task::LINUX_PRIORITY_REALTIME_MAX;
    const int nice_min = Os::Posix::Task::LINUX_PRIORITY_NICE_MIN;
    const int nice_zero = Os::Posix::Task::LINUX_PRIORITY_NICE_ZERO;
    const int max = Os::Posix::Task::LINUX_PRIORITY_MAX;
    ASSERT_EQ(sched_get_priority_max(SCHED_RR), realtime_base);
    ASSERT_EQ(sched_get_priority_min(SCHED_RR), realtime_base - realtime_max);
    ASSERT_EQ(sched_get_priority_min(SCHED_OTHER), 0);
    ASSERT_EQ(sched_get_priority_max(SCHED_OTHER), 0);
    ASSERT_EQ(nice_zero - nice_min, 20);
    ASSERT_EQ(max - nice_zero, 19);
    // Sentinels lie outside the Linux range and are never mapped
    ASSERT_GT(static_cast<int>(Os::Posix::Task::PosixTask::TASK_PRIORITY_NON_REALTIME), max);
    ASSERT_GT(static_cast<int>(Os::Task::TASK_PRIORITY_DEFAULT), max);
}

namespace {
//! Request and result of applying a nice value from within a task
struct NiceProbe {
    int request = 0;
    std::atomic<int> status{-1};
    std::atomic<int> nice{INT_MIN};
};

//! Task routine applying the requested nice value to itself and recording the outcome
void applyNiceRoutine(void* pointer) {
    NiceProbe& probe = *static_cast<NiceProbe*>(pointer);
    probe.status = Os::Posix::Task::set_task_nice(probe.request);
    probe.nice = currentNice();
}
}  // namespace

// set_task_nice applies to the calling thread only: the task sees the new value and this thread is unaffected
TEST_F(PosixTaskScheduling, SetTaskNiceAppliesToCallingThreadOnly) {
    const int original_nice = currentNice();
    NiceProbe probe;
    probe.request = raisedNice(5);
    Os::Task task;
    Fw::String name("NiceTask");
    Os::Task::Arguments arguments(name, applyNiceRoutine, &probe, Os::Task::TASK_PRIORITY_DEFAULT);
    ASSERT_EQ(task.start(arguments), Os::Task::Status::OP_OK);
    ASSERT_EQ(task.join(), Os::Task::Status::OP_OK);
#if defined(TGT_OS_TYPE_LINUX)
    ASSERT_EQ(probe.status.load(), 0);
    ASSERT_EQ(probe.nice.load(), probe.request);
    ASSERT_EQ(currentNice(), original_nice);
#else
    ASSERT_EQ(probe.status.load(), ENOTSUP);
#endif
}

#if defined(POSIX_THREADS_USE_LINUX_PRIORITIES) && POSIX_THREADS_USE_LINUX_PRIORITIES
// Linux priority mode: the nice band runs under SCHED_OTHER at the mapped nice; raising nice needs no privilege
TEST_F(PosixTaskScheduling, LinuxNiceBandTaskUsesSchedOtherAndNice) {
    const int nice = raisedNice(5);
    const FwTaskPriorityType priority =
        static_cast<FwTaskPriorityType>(Os::Posix::Task::LINUX_PRIORITY_NICE_ZERO + nice);
    ObservedSchedule observed;
    runAndObserve(priority, observed);
    ASSERT_EQ(observed.policy.load(), SCHED_OTHER);
    ASSERT_EQ(observed.priority.load(), 0);
    ASSERT_EQ(observed.nice.load(), nice);
}

// Linux priority mode: priorities beyond 139 clamp to nice 19
TEST_F(PosixTaskScheduling, LinuxClampedPriorityTaskUsesLowestNice) {
    ObservedSchedule observed;
    runAndObserve(200, observed);
    ASSERT_EQ(observed.policy.load(), SCHED_OTHER);
    ASSERT_EQ(observed.nice.load(), 19);
}

// Linux priority mode: a nice below the inherited one is applied when privileged and otherwise inherited
TEST_F(PosixTaskScheduling, LinuxNegativeNiceTaskAppliesOrInheritsNice) {
    const int inherited_nice = currentNice();
    ObservedSchedule observed;
    runAndObserve(110, observed);
    ASSERT_EQ(observed.policy.load(), SCHED_OTHER);
    if (geteuid() == 0) {
        ASSERT_EQ(observed.nice.load(), -10);
    } else {
        ASSERT_TRUE(observed.nice.load() == -10 || observed.nice.load() == inherited_nice);
    }
}

namespace {
//! Parent task state: the parent raises its own nice, then starts a nice-zero child and records the child's schedule
struct NestedNice {
    int parent_nice = 0;
    int parent_status = -1;
    ObservedSchedule child;
};

void parentRaisesNiceRoutine(void* pointer) {
    NestedNice& nested = *static_cast<NestedNice*>(pointer);
    nested.parent_status = Os::Posix::Task::set_task_nice(nested.parent_nice);
    Os::Task child;
    Fw::String name("ChildTask");
    Os::Task::Arguments arguments(name, recordScheduleRoutine, &nested.child,
                                  Os::Posix::Task::LINUX_PRIORITY_NICE_ZERO);
    if (child.start(arguments) == Os::Task::Status::OP_OK) {
        (void)child.join();
    }
}
}  // namespace

// Linux priority mode: the mapped nice is set explicitly rather than inherited from a nicer parent. Lowering the
// child's nice below the parent's requires privilege, so this is skipped when not running as root.
TEST_F(PosixTaskScheduling, LinuxNiceZeroTaskDoesNotInheritParentNice) {
    if (geteuid() != 0) {
        GTEST_SKIP() << "Insufficient permission to lower a nice value";
    }
    NestedNice nested;
    nested.parent_nice = raisedNice(3);
    Os::Task parent;
    Fw::String name("ParentTask");
    Os::Task::Arguments arguments(name, parentRaisesNiceRoutine, &nested, Os::Task::TASK_PRIORITY_DEFAULT);
    ASSERT_EQ(parent.start(arguments), Os::Task::Status::OP_OK);
    ASSERT_EQ(parent.join(), Os::Task::Status::OP_OK);
    ASSERT_EQ(nested.parent_status, 0);
    ASSERT_TRUE(nested.child.done);
    ASSERT_EQ(nested.child.policy.load(), SCHED_OTHER);
    ASSERT_EQ(nested.child.nice.load(), 0);
}

// Linux priority mode: the realtime band runs under SCHED_RR at the inverted priority when permitted, otherwise it
// falls back to inherited scheduling
TEST_F(PosixTaskScheduling, LinuxRealtimeBandTaskUsesSchedRrOrFallsBack) {
    int expected_policy = -1;
    sched_param expected_param;
    expected_param.sched_priority = -1;
    ASSERT_EQ(pthread_getschedparam(pthread_self(), &expected_policy, &expected_param), 0);

    ObservedSchedule highest;
    runAndObserve(0, highest);
    ObservedSchedule lowest;
    runAndObserve(98, lowest);
    if (geteuid() == 0 || highest.policy.load() == SCHED_RR) {
        ASSERT_EQ(highest.policy.load(), SCHED_RR);
        ASSERT_EQ(highest.priority.load(), 99);
        ASSERT_EQ(lowest.policy.load(), SCHED_RR);
        ASSERT_EQ(lowest.priority.load(), 1);
    } else {
        ASSERT_EQ(highest.policy.load(), expected_policy);
        ASSERT_EQ(highest.priority.load(), expected_param.sched_priority);
        ASSERT_EQ(lowest.policy.load(), expected_policy);
        ASSERT_EQ(lowest.priority.load(), expected_param.sched_priority);
    }
}
#else
// Default mode: a priority in the Linux nice band is still a (clamped) SCHED_RR priority, or falls back to inherited
TEST_F(PosixTaskScheduling, DefaultModeNiceBandPriorityRemainsSchedRr) {
    const FwTaskPriorityType priority = Os::Posix::Task::LINUX_PRIORITY_NICE_ZERO;
    ObservedSchedule observed;
    runAndObserve(priority, observed);
    if (geteuid() == 0 || observed.policy.load() == SCHED_RR) {
        ASSERT_EQ(observed.policy.load(), SCHED_RR);
        ASSERT_EQ(observed.priority.load(), sched_get_priority_max(SCHED_RR));
    } else {
        int expected_policy = -1;
        sched_param expected_param;
        expected_param.sched_priority = -1;
        ASSERT_EQ(pthread_getschedparam(pthread_self(), &expected_policy, &expected_param), 0);
        ASSERT_EQ(observed.policy.load(), expected_policy);
        ASSERT_EQ(observed.priority.load(), expected_param.sched_priority);
    }
}
#endif

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
