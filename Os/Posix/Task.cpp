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

    void validate_arguments(NATIVE_UINT_TYPE& priority, NATIVE_UINT_TYPE& stack, NATIVE_UINT_TYPE& affinity, bool expect_perm) {
        const NATIVE_INT_TYPE min_priority = sched_get_priority_min(SCHED_POLICY);
        const NATIVE_INT_TYPE max_priority = sched_get_priority_max(SCHED_POLICY);
        // Check to ensure that these calls worked.  -1 is an error
        if (min_priority < 0 or max_priority < 0) {
            Fw::Logger::logMsg("[WARNING] Unable to determine min/max priority with error %s. Discarding priority.\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
            priority = Os::Task::TASK_DEFAULT;
        }
        // Check priority attributes
        if (!expect_perm and priority != Task::TASK_DEFAULT) {
            Fw::Logger::logMsg("[WARNING] Task priority set and permissions unavailable. Discarding priority.\n");
            priority = Task::TASK_DEFAULT; //Action: use constant
        }
        if (priority != Task::TASK_DEFAULT and priority < static_cast<NATIVE_UINT_TYPE>(min_priority)) {
            Fw::Logger::logMsg("[WARNING] Low task priority of %d being clamped to %d\n", priority, min_priority);
            priority = min_priority;
        }
        if (priority != Task::TASK_DEFAULT and priority > static_cast<NATIVE_UINT_TYPE>(max_priority)) {
            Fw::Logger::logMsg("[WARNING] High task priority of %d being clamped to %d\n", priority, max_priority);
            priority = max_priority;
        }
        // Check the stack
        if (stack != Task::TASK_DEFAULT and stack < PTHREAD_STACK_MIN) {
            Fw::Logger::logMsg("[WARNING] Stack size %d too small, setting to minimum of %d\n", stack, PTHREAD_STACK_MIN);
            stack = PTHREAD_STACK_MIN;
        }
        // Check CPU affinity
        if (!expect_perm and affinity != Task::TASK_DEFAULT) {
            Fw::Logger::logMsg("[WARNING] Cpu affinity set and permissions unavailable. Discarding affinity.\n");
            affinity = Task::TASK_DEFAULT;
        }
    }

    Task::TaskStatus set_stack_size(pthread_attr_t& att, NATIVE_UINT_TYPE stack) {
        // Set the stack size, if it has been supplied
        if (stack != Task::TASK_DEFAULT) {
            I32 stat = pthread_attr_setstacksize(&att, stack);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_attr_setstacksize: %s\n", reinterpret_cast<POINTER_CAST>(strerror(stat)));
                return Task::TASK_INVALID_STACK;
            }
        }
        return Task::TASK_OK;
    }

    Task::TaskStatus set_priority_params(pthread_attr_t& att, NATIVE_UINT_TYPE priority) {
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
            schedParam.sched_priority = priority;
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

    Task::TaskStatus create_pthread(NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize, NATIVE_UINT_TYPE cpuAffinity, pthread_t*& tid, void* arg, bool expect_perm) {
        Task::TaskStatus tStat = Task::TASK_OK;
        validate_arguments(priority, stackSize, cpuAffinity, expect_perm);
        pthread_attr_t att;
        memset(&att,0, sizeof(att));


        I32 stat = pthread_attr_init(&att);
        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_init: (%d): %s\n", stat, reinterpret_cast<POINTER_CAST>(strerror(stat)));
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

    Task::TaskStatus Task::start(const Fw::StringBase &name, taskRoutine routine, void* arg, NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize,  NATIVE_UINT_TYPE cpuAffinity, NATIVE_UINT_TYPE identifier) {
        FW_ASSERT(routine);

        this->m_name = "TP_";
        this->m_name += name;
        this->m_identifier = identifier;
        // Setup functor wrapper parameters
        this->m_routineWrapper.routine = routine;
        this->m_routineWrapper.arg = arg;
        pthread_t* tid;

        // Try to create thread with assuming permissions
        TaskStatus status = create_pthread(priority, stackSize, cpuAffinity, tid, &this->m_routineWrapper, true);
        // Failure due to permission automatically retried
        if (status == TASK_ERROR_PERMISSION) {
            Fw::Logger::logMsg("[WARNING] Insufficient Permissions:\n");
            Fw::Logger::logMsg("[WARNING] Insufficient permissions to set task priority or set task CPU affinity on task %s. Creating task without priority nor affinity.\n", reinterpret_cast<POINTER_CAST>(m_name.toChar()));
            Fw::Logger::logMsg("[WARNING] Please use no-argument <component>.start() calls, set priority/affinity to TASK_DEFAULT or ensure user has correct permissions for operating system.\n");
            Fw::Logger::logMsg("[WARNING]      Note: future releases of fprime will fail when setting priority/affinity without sufficient permissions \n");
            Fw::Logger::logMsg("\n");
            status = create_pthread(priority, stackSize, cpuAffinity, tid, &this->m_routineWrapper, false); // Fallback with no permission
        }
        // Check for non-zero error code
        if (status != TASK_OK) {
            return status;
        }
        FW_ASSERT(tid != nullptr);

        // Handle a successfully created task
        this->m_handle = reinterpret_cast<POINTER_CAST>(tid);
        Task::s_numTasks++;
        // If a registry has been registered, register task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->addTask(this);
        }
        return status;
    }

    Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
    {
        timespec time1;

        time1.tv_sec = milliseconds/1000;
        time1.tv_nsec = (milliseconds%1000)*1000000;

        timespec time2;
        time2.tv_sec = 0;
        time2.tv_nsec = 0;

        timespec* sleepTimePtr = &time1;
        timespec* remTimePtr = &time2;

        while (true) {
            int stat = nanosleep(sleepTimePtr,remTimePtr);
            if (0 == stat) {
                return TASK_OK;
            } else { // check errno
                if (EINTR == errno) { // swap pointers
                    timespec* temp = remTimePtr;
                    remTimePtr = sleepTimePtr;
                    sleepTimePtr = temp;
                    continue; // if interrupted, just continue
                } else {
                    return TASK_DELAY_ERROR;
                }
            }
        }
        return TASK_OK; // for coverage analysis
    }


    Task::~Task() {
    	if (this->m_handle) {
    		delete reinterpret_cast<pthread_t*>(this->m_handle);
    	}
        // If a registry has been registered, remove task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->removeTask(this);
        }

    }

    // Note: not implemented for Posix threads. Must be manually done using a mutex or other blocking construct as there
    // is not top-level pthreads support for suspend and resume.

    void Task::suspend(bool onPurpose) {
        FW_ASSERT(0);
    }

    void Task::resume() {
        FW_ASSERT(0);
    }

    bool Task::isSuspended() {
        FW_ASSERT(0);
        return false;
    }

    TaskId Task::getOsIdentifier() {
        TaskId T;
        return T;
    }

    Task::TaskStatus Task::join(void **value_ptr) {
        NATIVE_INT_TYPE stat = 0;
        if (!(this->m_handle)) {
            return TASK_JOIN_ERROR;
        }
        stat = pthread_join(*reinterpret_cast<pthread_t*>(this->m_handle), value_ptr);

        if (stat != 0) {
            return TASK_JOIN_ERROR;
        }
        else {
            return TASK_OK;
        }
    }
}
