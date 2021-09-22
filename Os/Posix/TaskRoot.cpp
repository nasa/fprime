#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>


#ifdef TGT_OS_TYPE_VXWORKS
    #include <vxWorks.h>
    #include <taskLib.h> // need it for VX_FP_TASK
#else
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include <Fw/Logger/Logger.hpp>

#include <pthread.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <new>

//#define DEBUG_PRINT(x,...) Fw::Logger::logMsg(x,##__VA_ARGS__);
#define DEBUG_PRINT(x,...)

typedef void* (*pthread_func_ptr)(void*);

namespace Os {
    Task::Task() : m_handle(0), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false) {
    }

    Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity) {

    	// for linux, task names can only be of length 15, so just setting it to the name:
    	this->m_name = name;
        this->m_identifier = identifier;

        Task::TaskStatus tStat = TASK_OK;

        pthread_attr_t att;
        // clear att; can cause issues
        memset(&att,0,sizeof(att));

        I32 stat = pthread_attr_init(&att);
        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_init: (%d)(%d): %s\n",stat,errno,strerror(stat));
        	return TASK_INVALID_PARAMS;
        }

        stat = pthread_attr_setstacksize(&att,stackSize);
        if (stat != 0) {
            return TASK_INVALID_STACK;
        }

        stat = pthread_attr_setschedpolicy(&att,SCHED_RR);

        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_setschedpolicy: %s\n",strerror(errno));
            return TASK_INVALID_PARAMS;
        }
        stat = pthread_attr_setinheritsched(&att,PTHREAD_EXPLICIT_SCHED); // may not need this
        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_setinheritsched: %s\n",strerror(errno));
         	return TASK_INVALID_PARAMS;
        }
        sched_param schedParam;
        memset(&schedParam,0,sizeof(sched_param));
        schedParam.sched_priority = priority;
        stat = pthread_attr_setschedparam(&att,&schedParam);
        if (stat != 0) {
            Fw::Logger::logMsg("pthread_attr_setschedparam: %s\n",strerror(errno));
        	return TASK_INVALID_PARAMS;
        }

        // Set affinity before creating thread:
        if (cpuAffinity != -1) {

            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cpuAffinity,&cpuset);

            stat = pthread_attr_setaffinity_np(&att, sizeof(cpu_set_t), &cpuset);
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_setaffinity_np: %i %s\n",cpuAffinity,strerror(stat));
                return TASK_INVALID_PARAMS;
            }
        }

        // If a registry has been registered, register task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->addTask(this);
        }

        pthread_t* tid = new(std::nothrow) pthread_t;
        if (tid == nullptr) {
            Fw::Logger::logMsg("failed to allocate pthread_t\n");
            return TASK_UNKNOWN_ERROR;
        }

        stat = pthread_create(tid,&att,(pthread_func_ptr)routine,arg);

        switch (stat) {
            case 0:
                this->m_handle = reinterpret_cast<POINTER_CAST>(tid);
                Task::s_numTasks++;
                break;
            case EINVAL:
                delete tid;
                Fw::Logger::logMsg("pthread_create: %s\n",strerror(errno));
                tStat = TASK_INVALID_PARAMS;
                break;
            default:
                delete tid;
                tStat = TASK_UNKNOWN_ERROR;
                break;
        }

        if (tStat == TASK_OK) {

            stat = pthread_setname_np(*tid,this->m_name.toChar());
            if (stat != 0) {
                Fw::Logger::logMsg("pthread_setname_np: %s %s\n",this->m_name.toChar(),strerror(stat));
                delete tid;
                tStat = TASK_INVALID_PARAMS;
            }
        }

        (void)pthread_attr_destroy(&att);

        return tStat;
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
    		delete (pthread_t*)this->m_handle;
    	}
        // If a registry has been registered, remove task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->removeTask(this);
        }

    }

    // FIXME: Need to find out how to do this for Posix threads

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

    Task::TaskStatus Task::join(void **value_ptr) {
        NATIVE_INT_TYPE stat = 0;
        if (!(this->m_handle)) {
            return TASK_JOIN_ERROR;
        }
        stat = pthread_join(*((pthread_t*) this->m_handle), value_ptr);

        if (stat != 0) {
            DEBUG_PRINT("join: %s\n", strerror(errno));
            return TASK_JOIN_ERROR;
        }
        else {
            return TASK_OK;
        }
    }
}
