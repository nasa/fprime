// ======================================================================
// \title Os/Posix/LinuxSchedulingHelper.cpp
// \brief Linux-defined task priorities for the Posix implementation of Os::Task
// ======================================================================
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

FwTaskPriorityType linux_to_posix_priority(const CHAR* name, const FwTaskPriorityType priority) {
    FW_ASSERT(name != nullptr);
    FwTaskPriorityType clamped = priority;
    if (priority == LINUX_PRIORITY_UNOCCUPIED) {
        clamped = LINUX_PRIORITY_REALTIME_MAX;
    } else if ((priority > LINUX_PRIORITY_MAX) && (priority != Os::Task::TASK_PRIORITY_DEFAULT) &&
               (priority != PosixTask::TASK_PRIORITY_NON_REALTIME)) {
        clamped = LINUX_PRIORITY_MAX;
    }
    if (clamped != priority) {
        Fw::Logger::log("[WARNING] %s task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                        const_cast<CHAR*>(name), static_cast<FwSizeType>(priority), static_cast<FwSizeType>(clamped));
    }
    FwTaskPriorityType posix_priority = clamped;
    if (clamped <= LINUX_PRIORITY_REALTIME_MAX) {
        posix_priority = static_cast<FwTaskPriorityType>(LINUX_PRIORITY_REALTIME_MAX + 1 - clamped);
    } else if (clamped <= LINUX_PRIORITY_MAX) {
        // The nice band has no Posix priority: SCHED_OTHER is the single Posix priority 0 on Linux, with the nice
        // value applied afterwards by apply_linux_nice. The sentinel is the only way to request SCHED_OTHER from
        // Os::Posix::Task::create today; replace this with a direct SCHED_OTHER request once the sentinel is retired.
        posix_priority = PosixTask::TASK_PRIORITY_NON_REALTIME;
    }
    return posix_priority;
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
