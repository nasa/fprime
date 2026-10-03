// ======================================================================
// \title Os/Posix/LinuxSchedulingHelper.hpp
// \brief Linux-defined task priorities for the Posix implementation of Os::Task
// ======================================================================
#ifndef Os_Posix_LinuxSchedulingHelper_hpp_
#define Os_Posix_LinuxSchedulingHelper_hpp_

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

//! \brief convert a Linux-defined priority to the Posix priority understood by Os::Posix::Task
//!
//! The realtime band 0-98 becomes the SCHED_RR priority 99-1 (99 is clamped to 98 with a warning); the nice band
//! 100-139 becomes TASK_PRIORITY_NON_REALTIME (SCHED_OTHER), with the nice value applied later by apply_linux_nice
//! (above 139 is clamped to 139 with a warning). TASK_PRIORITY_DEFAULT and TASK_PRIORITY_NON_REALTIME are unchanged.
//!
//! \param name: task name used in warnings
//! \param priority: Linux-defined priority
//! \return Posix priority or sentinel
FwTaskPriorityType linux_to_posix_priority(const CHAR* name, FwTaskPriorityType priority);

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
