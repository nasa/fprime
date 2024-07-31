// ======================================================================
// \title Os/Posix/Task.cpp
// \brief implementation of Posix implementation of Os::Task
// ======================================================================
#include <cstring>
#include <unistd.h>
#include <climits>
#include <cerrno>
#include <pthread.h>

#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Task.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/Posix/error.hpp"

namespace Os {
namespace Posix {
namespace Task {
    std::atomic<bool> PosixTask::s_permissions_reported(false);
    static const PlatformIntType SCHED_POLICY = SCHED_RR;

    typedef void* (*pthread_func_ptr)(void*);

    void* pthread_entry_wrapper(void* wrapper_pointer) {
        FW_ASSERT(wrapper_pointer != nullptr);
        Os::Task::TaskRoutineWrapper& wrapper = *reinterpret_cast<Os::Task::TaskRoutineWrapper*>(wrapper_pointer);
        wrapper.run(&wrapper);
        return nullptr;
    }

    PlatformIntType set_stack_size(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        PlatformIntType status = PosixTaskHandle::SUCCESS;
        FwSizeType stack = arguments.m_stackSize;
        // Check for stack size multiple of page size
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size <= 0) {
            Fw::Logger::logMsg(
                    "[WARNING] %s could not determine page size %s. Skipping stack-size check.\n",
                    reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())),
                    reinterpret_cast<PlatformPointerCastType>(strerror(errno))
            );
        }
        else if ((stack % static_cast<FwSizeType>(page_size)) != 0) {
            // Round-down to nearest page size multiple
            FwSizeType rounded = (stack / static_cast<FwSizeType>(page_size)) * static_cast<FwSizeType>(page_size);
            Fw::Logger::logMsg(
                    "[WARNING] %s stack size of %" PRI_FwSizeType " is not multiple of page size %ld, rounding to %" PRI_FwSizeType "\n",
                    reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())),
                    static_cast<PlatformPointerCastType>(stack),
                    static_cast<PlatformPointerCastType>(page_size),
                    static_cast<PlatformPointerCastType>(rounded)
            );
            stack = rounded;
        }

        // Clamp invalid stack sizes
        if (stack <= static_cast<FwSizeType>(PTHREAD_STACK_MIN)) {
            Fw::Logger::logMsg(
                    "[WARNING] %s stack size of %" PRI_FwSizeType "  is too small, clamping to %" PRI_FwSizeType "\n",
                    reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())),
                    static_cast<PlatformPointerCastType>(stack),
                    static_cast<PlatformPointerCastType>(static_cast<FwSizeType>(PTHREAD_STACK_MIN))
            );
            stack = static_cast<FwSizeType>(PTHREAD_STACK_MIN);
        }
        status = pthread_attr_setstacksize(&attributes, static_cast<size_t>(stack));
        return status;
    }

    PlatformIntType set_priority_params(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        const FwSizeType min_priority = static_cast<FwSizeType>(sched_get_priority_min(SCHED_POLICY));
        const FwSizeType max_priority = static_cast<FwSizeType>(sched_get_priority_max(SCHED_POLICY));
        PlatformIntType status = PosixTaskHandle::SUCCESS;
        FwSizeType priority = arguments.m_priority;
        // Clamp to minimum priority
        if (priority < min_priority) {
            Fw::Logger::logMsg("[WARNING] %s low task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                               reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())),
                               static_cast<PlatformPointerCastType>(priority),
                               static_cast<PlatformPointerCastType>(min_priority));
            priority = min_priority;
        }
        // Clamp to maximum priority
        else if (priority > max_priority) {
            Fw::Logger::logMsg("[WARNING] %s high task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                               reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())),
                               static_cast<PlatformPointerCastType>(priority),
                               static_cast<PlatformPointerCastType>(max_priority));
            priority = max_priority;
        }

        // Set attributes required for priority
        status = pthread_attr_setschedpolicy(&attributes, SCHED_POLICY);
        if (status == PosixTaskHandle::SUCCESS) {
            status = pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
        }
        if (status == PosixTaskHandle::SUCCESS) {
            sched_param schedParam;
            memset(&schedParam, 0, sizeof(sched_param));
            schedParam.sched_priority = static_cast<PlatformIntType>(priority);
            status = pthread_attr_setschedparam(&attributes, &schedParam);
        }
        return status;
    }

    PlatformIntType set_cpu_affinity(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        PlatformIntType status = 0;
// Feature set check for _GNU_SOURCE before using GNU only features
#ifdef _GNU_SOURCE
        const FwSizeType affinity = arguments.m_cpuAffinity;
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(static_cast<PlatformIntType>(affinity), &cpu_set);

        // According to the man-page this function sets errno rather than returning an error status like other functions
        status = pthread_attr_setaffinity_np(&attributes, sizeof(cpu_set_t), &cpu_set);
        status = (status == PosixTaskHandle::SUCCESS) ? status : errno;
#else
            Fw::Logger::logMsg("[WARNING] %s setting CPU affinity is only available with GNU pthreads\n",
                               reinterpret_cast<PlatformPointerCastType>(const_cast<CHAR*>(arguments.m_name.toChar())));
#endif
        return status;
    }

    Os::Task::Status PosixTask::create(const Os::Task::Arguments& arguments, const PosixTask::PermissionExpectation permissions) {
        PlatformIntType pthread_status = PosixTaskHandle::SUCCESS;
        PosixTaskHandle& handle = this->m_handle;
        const bool expect_permission = (permissions == EXPECT_PERMISSION);
        // Initialize and clear pthread attributes
        pthread_attr_t attributes;
        memset(&attributes, 0, sizeof(attributes));
        pthread_status = pthread_attr_init(&attributes);
        if ((arguments.m_stackSize != Os::Task::TASK_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_stack_size(attributes, arguments);
        }
        if ((arguments.m_priority != Os::Task::TASK_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_priority_params(attributes, arguments);
        }
        if ((arguments.m_cpuAffinity != Os::Task::TASK_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_cpu_affinity(attributes, arguments);
        }
        if (pthread_status == PosixTaskHandle::SUCCESS) {
            pthread_status = pthread_create(&handle.m_task_descriptor, &attributes, pthread_entry_wrapper,
                                            arguments.m_routine_argument);
        }
        // Successful execution of all precious steps will result in a valid task handle
        if (pthread_status == PosixTaskHandle::SUCCESS) {
            handle.m_is_valid = true;
        }

        (void) pthread_attr_destroy(&attributes);
        return Posix::posix_status_to_task_status(pthread_status);
    }

    void PosixTask::onStart() {}

    Os::Task::Status PosixTask::start(const Arguments& arguments) {
        FW_ASSERT(arguments.m_routine != nullptr);

        // Try to create thread with assuming permissions
        Os::Task::Status status = this->create(arguments, PermissionExpectation::EXPECT_PERMISSION);
        // Failure due to permission automatically retried
        if (status == Os::Task::Status::ERROR_PERMISSION) {
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
                PosixTask::s_permissions_reported = true;
            }
            // Fallback with no permission
            status = this->create(arguments, PermissionExpectation::EXPECT_NO_PERMISSION);
        } else if (status != Os::Task::Status::OP_OK) {
            Fw::Logger::logMsg("[ERROR] Failed to create task with status: %d",
                               static_cast<PlatformPointerCastType>(static_cast<PlatformIntType>(status)));
        }
        return status;
    }

    Os::Task::Status PosixTask::join() {
        Os::Task::Status status = Os::Task::Status::JOIN_ERROR;
        if (not this->m_handle.m_is_valid) {
            status = Os::Task::Status::INVALID_HANDLE;
        } else {
            PlatformIntType stat = ::pthread_join(this->m_handle.m_task_descriptor, nullptr);
            status = (stat == PosixTaskHandle::SUCCESS) ? Os::Task::Status::OP_OK : Os::Task::Status::JOIN_ERROR;
        }
        return status;
    }

    TaskHandle* PosixTask::getHandle() {
        return &this->m_handle;
    }

    // Note: not implemented for Posix threads. Must be manually done using a mutex or other blocking construct as there
    // is no top-level pthreads support for suspend and resume.
    void PosixTask::suspend(Os::Task::SuspensionType suspensionType) {
        FW_ASSERT(0);
    }

    void PosixTask::resume() {
        FW_ASSERT(0);
    }
} // end namespace Task
} // end namespace Posix
} // end namespace Os
