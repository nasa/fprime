// ======================================================================
// \title Os/Posix/LinuxSchedulingHelper.hpp
// \brief Linux-defined task priorities for the Posix implementation of Os::Task
// ======================================================================
#ifndef Os_Posix_LinuxSchedulingHelper_hpp_
#define Os_Posix_LinuxSchedulingHelper_hpp_

#include <pthread.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Task.hpp>

#if !defined(TGT_OS_TYPE_LINUX)
#error "POSIX_THREADS_USE_LINUX_PRIORITIES requires a Linux target"
#endif

namespace Os {
namespace Posix {
namespace Task {

//! Linux-defined task priorities run 0 (highest) through 139 (lowest): 0-98 are the SCHED_RR priorities 99-1, 99 is
//! unoccupied, and 100-139 are SCHED_OTHER at nice -20 through 19.
static constexpr FwTaskPriorityType LINUX_PRIORITY_REALTIME_MAX = 98;  //!< Last SCHED_RR priority (SCHED_RR 1)
static constexpr FwTaskPriorityType LINUX_PRIORITY_NICE_MIN = 100;     //!< First SCHED_OTHER priority (nice -20)
static constexpr FwTaskPriorityType LINUX_PRIORITY_NICE_ZERO = 120;    //!< SCHED_OTHER priority at nice 0
static constexpr FwTaskPriorityType LINUX_PRIORITY_MAX = 139;          //!< Last SCHED_OTHER priority (nice 19)

//! \brief clamp a Linux-defined priority onto an occupied level, warning when it changes
//!
//! \param name: task name used in the warning
//! \param priority: Linux-defined priority
//! \return priority 0-98 or 100-139
FwTaskPriorityType clamp_linux_priority(const CHAR* name, FwTaskPriorityType priority);

//! \brief set the scheduling attributes for a Linux-defined priority
//!
//! The realtime band (SCHED_RR) is only set when permission is expected; the nice band (SCHED_OTHER) needs none.
//!
//! \param attributes: pthread attributes to set
//! \param arguments: task arguments supplying the name and priority
//! \param expect_permission: whether realtime scheduling permission is expected
//! \return 0 on success, otherwise the pthread error
int set_linux_priority_params(pthread_attr_t& attributes, const Os::Task::Arguments& arguments, bool expect_permission);

//! \brief apply the nice value of a Linux-defined priority to the calling thread
//!
//! Does nothing for the realtime band or for the TASK_PRIORITY_DEFAULT and TASK_PRIORITY_NON_REALTIME sentinels.
//! Lowering nice requires permission; without it the thread keeps its inherited nice after a one-time note.
//!
//! \param name: task name used in warnings
//! \param priority: Linux-defined priority
void apply_linux_nice(const CHAR* name, FwTaskPriorityType priority);

}  // namespace Task
}  // namespace Posix
}  // namespace Os
#endif  // Os_Posix_LinuxSchedulingHelper_hpp_
