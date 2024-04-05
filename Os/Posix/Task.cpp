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
#include "Os/Posix/error.hpp"

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



    Task::Status PosixTask::create(const Task::Arguments& arguments, const PosixTask::PermissionExpectation permissions) {
        Task::Status status = Task::OP_OK;
        PlatformIntType pthread_status = 0;
        PosixTaskHandle& handle = this->m_handle;

        // TODO: validate arguments???

        // Initialize and clear pthread attributes
        pthread_attr_t attributes;
        memset(&attributes, 0, sizeof(attributes));
        pthread_status = pthread_attr_init(&attributes);

        // Check for valid status before continuing
        if (pthread_status == 0) {
            pthread_status = set_stack_size();
            if (pthread_status == PosixTaskHandle::SUCCESS_RETURN_VALUE) {
                pthread_status = set_priority_params();
                if (pthread_status == PosixTaskHandle::SUCCESS_RETURN_VALUE) {
                    pthread_status = set_cpu_affinity();
                    if (pthread_status == PosixTaskHandle::SUCCESS_RETURN_VALUE) {
                        pthread_status = pthread_create(handle.m_task_descriptor, &attributes, pthread_entry_wrapper,
                                                        arg);
                        if (pthread_status == PosixTaskHandle::SUCCESS_RETURN_VALUE) {
                            handle.m_is_valid = true;
                        }
                    }
                }
            }
        }
        (void) pthread_attr_destroy(&attributes);
        return Posix::posix_status_to_task_status(pthread_status);

    }


    Task::Status PosixTask::start(const Arguments& arguments) {
        FW_ASSERT(arguments.m_routine != nullptr);
        pthread_t* thread_handle = nullptr;

        // Try to create thread with assuming permissions
        Task::Status status = this->create(arguments, PermissionExpectation::EXPECT_PERMISSION);
        // Failure due to permission automatically retried
        if (status == Task::Status::ERROR_PERMISSION) {
            if (not PosixTask::s_permissions_reported) {
                Fw::Logger::logMsg("\n");
                Fw::Logger::logMsg("[NOTE] Task Permissions:\n");
                Fw::Logger::logMsg("[NOTE]\n");
                Fw::Logger::logMsg("[NOTE] You have insufficient permissions to create a task with priority and/or cpu affinity.\n");
                Fw::Logger::logMsg("[NOTE] A task without priority and affinity will be created.\n");
                Fw::Logger::logMsg("[NOTE]\n");
                Fw::Logger::logMsg("[NOTE] There are three possible resolutions:\n");
                Fw::Logger::logMsg("[NOTE] 1. Use tasks without priority and affinity using parameterless start()\n");
                Fw::Logger::logMsg("[NOTE] 2. Run this executable as a user with task priority permission\n");
                Fw::Logger::logMsg("[NOTE] 3. Grant capability with \"setcap 'cap_sys_nice=eip'\" or equivalent\n");
                Fw::Logger::logMsg("\n");
            }
            // Fallback with no permission
            status = this->create(arguments, PermissionExpectation::EXPECT_NO_PERMISSION);
        }
        return status;
    }

    Task::Status PosixTask::join() {
        Task::Status status = Task::Status::JOIN_ERROR;
        if (not this->m_handle.m_is_valid) {
            status = Task::Status::INVALID_HANDLE;
        } else {
            PlatformIntType stat = ::pthread_join(this->m_handle.m_task_descriptor, nullptr);
            status = (stat == PosixTaskHandle::SUCCESS_RETURN_VALUE) ? Task::Status::OP_OK : Task::Status::JOIN_ERROR;
        }
        return status;
    }

    bool PosixTask::isCooperative() {
        return false;
    }

    TaskHandle* PosixTask::getHandle() {
        return &this->m_handle;
    }

    // Note: not implemented for Posix threads. Must be manually done using a mutex or other blocking construct as there
    // is no top-level pthreads support for suspend and resume.
    void PosixTask::suspend(Task::SuspensionType suspensionType) {
        FW_ASSERT(0);
    }

    void PosixTask::resume() {
        FW_ASSERT(0);
    }
}
