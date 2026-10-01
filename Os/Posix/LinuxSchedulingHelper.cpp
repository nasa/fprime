// ======================================================================
// \title Os/Posix/LinuxSchedulingHelper.cpp
// \brief Linux-defined task priorities for the Posix implementation of Os::Task
// ======================================================================
#include <sched.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <cstring>

#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Posix/LinuxSchedulingHelper.hpp"
#include "Os/Posix/Task.hpp"

namespace Os {
namespace Posix {
namespace Task {
static const FwTaskPriorityType LINUX_PRIORITY_UNOCCUPIED = 99;
static std::atomic<bool> s_nice_permissions_reported(false);

FwTaskPriorityType clamp_linux_priority(const CHAR* name, const FwTaskPriorityType priority) {
    FW_ASSERT(name != nullptr);
    FwTaskPriorityType clamped = priority;
    if (priority == LINUX_PRIORITY_UNOCCUPIED) {
        clamped = LINUX_PRIORITY_REALTIME_MAX;
    } else if (priority > LINUX_PRIORITY_MAX) {
        clamped = LINUX_PRIORITY_MAX;
    }
    if (clamped != priority) {
        Fw::Logger::log("[WARNING] %s task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                        const_cast<CHAR*>(name), static_cast<FwSizeType>(priority), static_cast<FwSizeType>(clamped));
    }
    return clamped;
}

int set_linux_priority_params(pthread_attr_t& attributes,
                              const Os::Task::Arguments& arguments,
                              const bool expect_permission) {
    int status = PosixTaskHandle::SUCCESS;
    const FwTaskPriorityType clamped = clamp_linux_priority(arguments.m_name.toChar(), arguments.m_priority);
    const bool realtime = (clamped <= LINUX_PRIORITY_REALTIME_MAX);
    // SCHED_OTHER has the single priority 0 on Linux and needs no permission; SCHED_RR does
    if (!realtime || expect_permission) {
        sched_param schedParam;
        (void)memset(&schedParam, 0, sizeof(sched_param));
        schedParam.sched_priority = realtime ? static_cast<int>(LINUX_PRIORITY_REALTIME_MAX + 1 - clamped) : 0;
        status = pthread_attr_setschedpolicy(&attributes, realtime ? SCHED_RR : SCHED_OTHER);
        if (status == PosixTaskHandle::SUCCESS) {
            status = pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
        }
        if (status == PosixTaskHandle::SUCCESS) {
            status = pthread_attr_setschedparam(&attributes, &schedParam);
        }
    }
    return status;
}

void apply_linux_nice(const CHAR* name, const FwTaskPriorityType priority) {
    FW_ASSERT(name != nullptr);
    if ((priority >= LINUX_PRIORITY_NICE_MIN) && (priority != Os::Task::TASK_PRIORITY_DEFAULT) &&
        (priority != PosixTask::TASK_PRIORITY_NON_REALTIME)) {
        const int nice =
            static_cast<int>(FW_MIN(priority, LINUX_PRIORITY_MAX)) - static_cast<int>(LINUX_PRIORITY_NICE_ZERO);
        // Nice is per thread on Linux, addressed by the kernel thread id
        const int status = (setpriority(PRIO_PROCESS, static_cast<id_t>(syscall(SYS_gettid)), nice) == 0)
                               ? PosixTaskHandle::SUCCESS
                               : errno;
        if ((status == EACCES) || (status == EPERM)) {
            if (not s_nice_permissions_reported.exchange(true)) {
                Fw::Logger::log("\n");
                Fw::Logger::log("[NOTE] Task Nice Permissions:\n");
                Fw::Logger::log("[NOTE]\n");
                Fw::Logger::log("[NOTE] You have insufficient permissions to lower a task's nice value below the\n");
                Fw::Logger::log("[NOTE] inherited value. Such tasks will run at the inherited nice value.\n");
                Fw::Logger::log("[NOTE] Run as a user with task priority permission, grant the capability with\n");
                Fw::Logger::log("[NOTE] \"setcap 'cap_sys_nice=eip'\", or use task priorities of %" PRI_FwSizeType
                                " plus the inherited nice or higher.\n",
                                static_cast<FwSizeType>(LINUX_PRIORITY_NICE_ZERO));
                Fw::Logger::log("\n");
            }
        } else if (status != PosixTaskHandle::SUCCESS) {
            Fw::Logger::log("[WARNING] %s nice value of %d not applied: %s\n", const_cast<CHAR*>(name), nice,
                            strerror(status));
        }
    }
}

}  // namespace Task
}  // namespace Posix
}  // namespace Os
