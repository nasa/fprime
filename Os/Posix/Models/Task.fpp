# ======================================================================
# \title Os/Posix/Models/Task.fpp
# \brief FPP definitions for Os/Posix/Task.hpp concepts (Posix platforms only)
# ======================================================================

module Os {
module Posix {
@ Sentinel task priority: run the task under SCHED_OTHER (non-realtime) instead of SCHED_RR
@ Casts to Os::Posix::Task::PosixTask::TASK_PRIORITY_NON_REALTIME. Requires no scheduling privileges.
constant TASK_PRIORITY_NON_REALTIME = -2
}
}
