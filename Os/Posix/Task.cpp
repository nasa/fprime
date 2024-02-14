#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

#include <pthread.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <new>
#include <sched.h>
#include <climits>
#include <Fw/Logger/Logger.hpp>

#ifdef TGT_OS_TYPE_LINUX
#include <features.h>
#endif

#include "Os/Posix/Task.hpp"

static const NATIVE_INT_TYPE SCHED_POLICY = SCHED_RR;

typedef void* (*pthread_func_ptr)(void*);

void* pthread_entry_wrapper(void* arg) {
    FW_ASSERT(arg);
    Os::Task::TaskRoutineWrapper *task = reinterpret_cast<Os::Task::TaskRoutineWrapper*>(arg);
    FW_ASSERT(task->routine);
    task->routine(task->arg);
    return nullptr;
}





namespace Os {
    bool PosixTask::s_permissions_reported = false;
    struct PriorityBounds {
        PriorityBounds() {
            // Check the priority bounds once
            if (s_bounds.m_min_priority < 0 or s_bounds.m_max_priority < 0) {
                Fw::Logger::logMsg("[WARNING] Min/max thread priorities unavailable: %s. Discarding user priorities.\n",
                                   reinterpret_cast<POINTER_CAST>(strerror(errno)));
            }
        }
        const PlatformIntType m_min_priority = ::sched_get_priority_min(SCHED_POLICY);
        const PlatformIntType m_max_priority = ::sched_get_priority_max(SCHED_POLICY);
        static PriorityBounds s_bounds;
    };


    void validate_arguments(Task::Arguments& arguments, bool no_permissions) {
        const PlatformIntType min_priority = sched_get_priority_min(SCHED_POLICY);
        const PlatformIntType max_priority = sched_get_priority_max(SCHED_POLICY);
        FwSizeType priority = arguments.m_priority;
        FwIndexType affinity = arguments.m_cpuAffinity;
        FwIndexType stack = arguments.m_cpuAffinity;
        // Check to ensure that these calls worked.  -1 is an error
        if (PriorityBounds::s_bounds::m_min_priority < 0 or PriorityBounds::s_bounds::m_max_priority < 0) {
            priority = Os::Task::TASK_DEFAULT;
        }
        // Check priority attributes
        if (!expect_perm and priority != Task::TASK_DEFAULT) {
            Fw::Logger::logMsg("[WARNING] Task priority set and permissions unavailable. Discarding priority.\n");
            priority = Task::TASK_DEFAULT; //Action: use constant
        }
        if (priority != Task::TASK_DEFAULT and priority < static_cast<Task::ParamType>(min_priority)) {
            Fw::Logger::logMsg("[WARNING] Low task priority of %d being clamped to %d\n", priority, static_cast<POINTER_CAST>(min_priority));
            priority = static_cast<NATIVE_UINT_TYPE>(min_priority);
        }
        if (priority != Task::TASK_DEFAULT and priority > static_cast<Task::ParamType>(max_priority)) {
            Fw::Logger::logMsg("[WARNING] High task priority of %d being clamped to %d\n", priority, static_cast<NATIVE_UINT_TYPE>(max_priority));
            priority = static_cast<NATIVE_UINT_TYPE>(max_priority);
        }
        // Check the stack
        if (stack != Task::TASK_DEFAULT and stack < PTHREAD_STACK_MIN) {
            Fw::Logger::logMsg("[WARNING] Stack size %d too small, setting to minimum of %d\n", stack, static_cast<NATIVE_UINT_TYPE>(PTHREAD_STACK_MIN));
            stack = static_cast<NATIVE_UINT_TYPE>(PTHREAD_STACK_MIN);
        }
        // Check CPU affinity
        if (!expect_perm and affinity != Task::TASK_DEFAULT) {
            Fw::Logger::logMsg("[WARNING] Cpu affinity set and permissions unavailable. Discarding affinity.\n");
            affinity = Task::TASK_DEFAULT;
        }
        else if (priority != static_cast<FwSizeType>(Task::TASK_DEFAULT)) {
            if (priority < static_cast<FwSizeType>(min_priority)) {
                Fw::Logger::logMsg("[WARNING] Low task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                                   priority, min_priority);
                priority = min_priority;
            } else if (priority < static_cast<FwSizeType>(min_priority)) {
                Fw::Logger::logMsg("[WARNING] High task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                                   priority, max_priority);
                priority = max_priority;
            }
        }

        // TODO: Change PTHREAD_STACK_MIN to constant
        if ((arguments.m_stackSize != Task::TASK_DEFAULT) and (arguments.m_stackSize < PTHREAD_STACK_MIN)) {
            Fw::Logger::logMsg("[WARNING] Stack size %" PRI_FwSizeType " too small, clamping to %" PRI_FwSizeType "\n",
                               arguments.m_stackSize, PTHREAD_STACK_MIN);
            stack = PTHREAD_STACK_MIN;
        }
    }

    Task::Status set_stack_size(pthread_attr_t& att, FwSizeType stack) {
        // Set the stack size, if it has been supplied
        if (stack != Task::TASK_DEFAULT) {
            I32 stat = pthread_attr_setstacksize(&att, static_cast<PlatformIntType>(stack));
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_attr_setstacksize: %s\n",
                                   reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::INVALID_STACK;
            }
        }
        return Task::OP_OK;
    }

    Task::Status set_priority_params(pthread_attr_t& att, FwSizeType priority) {
        if (priority != Task::TASK_DEFAULT) {
            I32 stat = pthread_attr_setschedpolicy(&att, SCHED_POLICY);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_attr_setschedpolicy: %s\n", reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::TASK_INVALID_PARAMS;
            }

            stat = pthread_attr_setinheritsched(&att, PTHREAD_EXPLICIT_SCHED);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_attr_setinheritsched: %s\n",
                                   reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::TASK_INVALID_PARAMS;
            }

            sched_param schedParam;
            memset(&schedParam, 0, sizeof(sched_param));
            schedParam.sched_priority =static_cast<int>(priority);
            stat = pthread_attr_setschedparam(&att, &schedParam);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_attr_setschedparam: %s\n", reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::TASK_INVALID_PARAMS;
            }
        }
        return Task::TASK_OK;
    }

    Task::TaskStatus set_cpu_affinity(pthread_attr_t& att, NATIVE_UINT_TYPE cpuAffinity) {
        if (cpuAffinity != Task::TASK_DEFAULT) {
#if TGT_OS_TYPE_LINUX && __GLIBC__
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cpuAffinity, &cpuset);

            I32 stat = pthread_attr_setaffinity_np(&att, sizeof(cpu_set_t), &cpuset);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_setaffinity_np: %i %s\n", cpuAffinity,
                                   reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::TASK_INVALID_PARAMS;
            }
#elif TGT_OS_TYPE_LINUX
            Fw::Logger::logMsg("[WARNING] Setting CPU affinity is only available on Linux with glibc\n");
#else
            Fw::Logger::logMsg("[WARNING] Setting CPU affinity is only available on Linux\n");
#endif
        }
        return Task::TASK_OK;
    }

    Task::TaskStatus create_pthread(Task::Arguments& arguments, bool try_permissions) {
        Task::Status status = Task::OP_OK;
        validate_arguments(arguments);
        pthread_attr_t att;
        memset(&att,0, sizeof(att));


        I32 stat = pthread_attr_init(&att);
        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_init: (%d): %s\n", static_cast<POINTER_CAST>(stat), reinterpret_cast<POINTER_CAST>(strerror(stat)));
            return Task::TASK_INVALID_PARAMS;
        }

        // Handle setting stack size
        tStat = set_stack_size(att, stackSize);
        if (tStat != Task::TASK_OK) {
            return tStat;
        }


        // Handle non-zero priorities
        tStat = set_priority_params(att, priority);
        if (tStat != Task::TASK_OK) {
            return tStat;
        }

        // Set affinity before creating thread:
        tStat = set_cpu_affinity(att, cpuAffinity);
        if (tStat != Task::TASK_OK) {
            return tStat;
        }

        tid = new pthread_t;
        const char* message = nullptr;

        stat = pthread_create(tid, &att, pthread_entry_wrapper, arg);
        switch (stat) {
            // Success, do nothing
            case 0:
                break;
            case EINVAL:
                message = "Invalid thread attributes specified";
                tStat = Task::TASK_INVALID_PARAMS;
                break;
            case EPERM:
                message = "Insufficient permissions to create thread. May not set thread priority without permission";
                tStat = Task::TASK_ERROR_PERMISSION;
                break;
            case EAGAIN:
                message = "Unable to allocate thread. Increase thread ulimit.";
                tStat = Task::TASK_ERROR_RESOURCES;
                break;
            default:
                message = "Unknown error";
                tStat = Task::TASK_UNKNOWN_ERROR;
                break;
        }
        (void)pthread_attr_destroy(&att);
        if (stat != 0) {
            delete tid;
            tid = nullptr;
            Fw::Logger::logMsg("pthread_create: %s. %s\n", reinterpret_cast<POINTER_CAST>(message), reinterpret_cast<POINTER_CAST>(strerror(stat)));
            return tStat;
        }
        return Task::TASK_OK;
    }

    Task::Task() : m_handle(reinterpret_cast<POINTER_CAST>(nullptr)), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false), m_routineWrapper() {
    }

    Task::Status PosixTask::start(const Arguments& arguments) {
        FW_ASSERT(arguments.m_routine != nullptr);
        pthread_t* thread_handle = nullptr;

        // Try to create thread with assuming permissions
        Task::Status status = create_pthread(arguments, true);
        // Failure due to permission automatically retried
        if (status == Task::Status::ERROR_PERMISSION) {
            if (not PosixTask::s_permissions_reported) {
                Fw::Logger::logMsg("[WARNING] Insufficient Task Permissions:\n");
                Fw::Logger::logMsg("[WARNING] Insufficient permissions to set task priorities or CPU affinities on task %s. Creating task without priority nor affinity.\n");
                Fw::Logger::logMsg("[WARNING] Please use no-argument <component>.start() calls, set priority/affinity to TASK_DEFAULT or ensure user has correct permissions.\n");
                Fw::Logger::logMsg("[WARNING]      Note: tasks will be created but priority will not be respected \n");
                Fw::Logger::logMsg("\n");
            }

            status = create_pthread(arguments, false); // Fallback with no permission
        } else if (status == Task::Status::OP_OK) {
            FW_ASSERT(thread_handle != nullptr);
            this->m_handle.m_task_descriptor = thread_handle;
        }
        return status;
    }

    // Note: not implemented for Posix threads. Must be manually done using a mutex or other blocking construct as there
    // is not top-level pthreads support for suspend and resume.

    void PosixTask::suspend(Task::SuspensionType suspensionType) {
        FW_ASSERT(0);
    }

    void PosixTask::resume() {
        FW_ASSERT(0);
    }

    Task::Status PosixTask::join() {
        Task::Status status = Task::Status::JOIN_ERROR;
        if (this->m_handle.m_task_descriptor != PosixTaskHandle::INVALID_THREAD_DESCRIPTOR) {
            status = Task::Status::INVALID_HANDLE;
        } else {
            PlatformIntType stat = ::pthread_join(*this->m_handle.m_task_descriptor, nullptr);
            status = (stat == PosixTaskHandle::SUCCESS_RETURN_VALUE) ? Task::Status::OP_OK : Task::Status::JOIN_ERROR;
        }
        return status;
    }
}
