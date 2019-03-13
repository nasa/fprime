#include <Os/Mutex.hpp>
#include <pthread.h>
#include <Fw/Types/Assert.hpp>

namespace Os {
    
    Mutex::Mutex(void) {
        pthread_mutex_t* handle = new pthread_mutex_t;
        // set attributes
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);

        NATIVE_INT_TYPE stat;
        // set to error checking
//        stat = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
//        FW_ASSERT(stat == 0,stat);

        // set to normal mutex type
        stat = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_NORMAL);
        FW_ASSERT(stat == 0,stat);

        // set to check for priority inheritance
        stat = pthread_mutexattr_setprotocol(&attr,PTHREAD_PRIO_INHERIT);
        FW_ASSERT(stat == 0,stat);

        stat = pthread_mutex_init(handle,&attr);
        FW_ASSERT(stat == 0,stat);

        this->m_handle = (POINTER_CAST) handle;
    }
    
    Mutex::~Mutex(void) {
        NATIVE_INT_TYPE stat = pthread_mutex_destroy((pthread_mutex_t*)this->m_handle);
        FW_ASSERT(stat == 0,stat);
        delete (pthread_mutex_t*)this->m_handle;
    }
    
    void Mutex::lock(void) {
        NATIVE_INT_TYPE stat = pthread_mutex_lock((pthread_mutex_t*)this->m_handle);
        FW_ASSERT(stat == 0,stat);
    }

    void Mutex::unLock(void) {
        NATIVE_INT_TYPE stat = pthread_mutex_unlock((pthread_mutex_t*)this->m_handle);
        FW_ASSERT(stat == 0,stat);
    }
            
}


