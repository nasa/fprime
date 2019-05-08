#include <Fw/Types/Assert.hpp>
#include <Os/Queue.hpp>

#ifdef TGT_OS_TYPE_VXWORKS
    #include <vxWorks.h>
#endif

#ifdef TGT_OS_TYPE_LINUX
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

namespace Os {

    class QueueHandle {
        public:
        QueueHandle(mqd_t m_handle) {
            // Initialize the handle:
            int ret;
            ret = pthread_cond_init(&this->queueNotEmpty, NULL);
            FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
            ret = pthread_cond_init(&this->queueNotFull, NULL);
            FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
            ret = pthread_mutex_init(&this->mp, NULL);
            FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
            this->handle = m_handle;
        }
        ~QueueHandle() { 
            // Destroy the handle:
            if (-1 != this->handle) {
                (void) mq_close(this->handle);
            }
            (void) pthread_cond_destroy(&this->queueNotEmpty);
            (void) pthread_mutex_destroy(&this->mp);
        }
        mqd_t handle;
        pthread_cond_t queueNotEmpty;
        pthread_cond_t queueNotFull;
        pthread_mutex_t mp;
    };

    Queue::Queue() :
        m_handle(-1) {
    }

    Queue::QueueStatus Queue::createInternal(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {

        this->m_name = "/QP_";
        this->m_name += name;
#ifndef TGT_OS_TYPE_VXWORKS
        char pid[40];
        (void)snprintf(pid,sizeof(pid),".%d",getpid());
        pid[sizeof(pid)-1] = 0;
        this->m_name += pid;
#endif
        mq_attr att;
        mqd_t handle;

        memset(&att,0,sizeof(att));
        att.mq_maxmsg = depth;
        att.mq_msgsize = msgSize;
        att.mq_flags = 0;
        att.mq_curmsgs = 0;

        handle = mq_open(this->m_name.toChar(), O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK, 0666, &att);

        // If queue already exists, then unlink it and try again.
        if (-1 == (NATIVE_INT_TYPE) handle) {
        	switch (errno) {
        	case EEXIST:
        		(void)mq_unlink(this->m_name.toChar());
        		break;
        	default:
            	return QUEUE_UNINITIALIZED;
        	}

        	handle = mq_open(this->m_name.toChar(), O_RDWR | O_CREAT | O_EXCL, 0666, &att);

            if (-1 == (NATIVE_INT_TYPE) handle) {
                return QUEUE_UNINITIALIZED;
            }
        }

        // Set up queue handle:
        QueueHandle* queueHandle = new QueueHandle(handle);
        if (NULL == queueHandle) {
          return QUEUE_UNINITIALIZED;
        }
        this->m_handle = (POINTER_CAST) queueHandle;
        
        Queue::s_numQueues++;

        return QUEUE_OK;
    }

    Queue::~Queue() {
        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        delete queueHandle;
        (void) mq_unlink(this->m_name.toChar());
    }

    Queue::QueueStatus Queue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {

        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        mqd_t handle = queueHandle->handle;
        pthread_cond_t* queueNotEmpty = &queueHandle->queueNotEmpty;
        pthread_cond_t* queueNotFull = &queueHandle->queueNotFull;
        pthread_mutex_t* mp = &queueHandle->mp;

        if (-1 == handle) {
            return QUEUE_UNINITIALIZED;
        }
        
        if (NULL == buffer) {
            return QUEUE_EMPTY_BUFFER;
        }

        bool keepTrying = true;
        int ret; 
        while (keepTrying) {
            NATIVE_INT_TYPE stat = mq_send(handle, (const char*) buffer, size, priority);
            if (-1 == stat) {
                switch (errno) {
                    case EINTR:
                        continue;
                    case EMSGSIZE:
                        return QUEUE_SIZE_MISMATCH;
                    case EINVAL:
                        return QUEUE_INVALID_PRIORITY;
                    case EAGAIN:
                        if (block == QUEUE_NONBLOCKING) {
                            // no more messages. If we are
                            // non-blocking, return
                            return QUEUE_FULL;
                        } else {
                            // Go to sleep until we receive a signal that something was takeng off the queue:
                            // Note: pthread_cont_wait must be called "with mutex locked by the calling 
                            // thread or undefined behavior results." - from the docs
                            ret = pthread_mutex_lock(mp);
                            FW_ASSERT(ret == 0, errno);
                            ret = pthread_cond_wait(queueNotFull, mp);
                            FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
                            ret = pthread_mutex_unlock(mp);
                            FW_ASSERT(ret == 0, errno);
                            continue;
                        }
                    default:
                        return QUEUE_UNKNOWN_ERROR;
                }
            } else {
                keepTrying=false;
                // Wake up a thread that might be waiting on the other end of the queue:
                ret = pthread_cond_signal(queueNotEmpty);
                FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
            }
        }
       
        return QUEUE_OK;
    }

    Queue::QueueStatus Queue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {

        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        mqd_t handle = queueHandle->handle;
        pthread_cond_t* queueNotEmpty = &queueHandle->queueNotEmpty;
        pthread_cond_t* queueNotFull = &queueHandle->queueNotFull;
        pthread_mutex_t* mp = &queueHandle->mp;

        if (-1 == handle) {
            return QUEUE_UNINITIALIZED;
        }

        ssize_t size;
        int ret; 
        bool notFinished = true;
        while (notFinished) {
            size = mq_receive(handle, (char*) buffer, (size_t) capacity,
#ifdef TGT_OS_TYPE_VXWORKS        		
                        (int*)&priority);
#else
                        (unsigned int*) &priority);
#endif        

            if (-1 == size) { // error
                switch (errno) {
                    case EINTR:
                        continue;
                    case EMSGSIZE:
                        return QUEUE_SIZE_MISMATCH;
                    case EAGAIN:
                        if (block == QUEUE_NONBLOCKING) {
                            // no more messages. If we are
                            // non-blocking, return
                            return QUEUE_NO_MORE_MSGS;
                        } else {
                            // Go to sleep until we receive a signal that something was put on the queue:
                            // Note: pthread_cont_wait must be called "with mutex locked by the calling 
                            // thread or undefined behavior results." - from the docs
                            ret = pthread_mutex_lock(mp);
                            FW_ASSERT(ret == 0, errno);
                            ret = pthread_cond_wait(queueNotEmpty, mp);
                            FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
                            ret = pthread_mutex_unlock(mp);
                            FW_ASSERT(ret == 0, errno);
                            continue;
                        }
                        break;
                    default:
                        return QUEUE_UNKNOWN_ERROR;
                }
            }
            else {
                notFinished = false;
                // Wake up a thread that might be waiting on the other end of the queue:
                ret = pthread_cond_signal(queueNotFull);
                FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
            }
        }

        actualSize = (NATIVE_INT_TYPE) size;
        return QUEUE_OK;
    }

    NATIVE_INT_TYPE Queue::getNumMsgs(void) const {
        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return (U32) attr.mq_curmsgs;
    }

    NATIVE_INT_TYPE Queue::getMaxMsgs(void) const {
        //FW_ASSERT(0);
        return 0;
    }

    NATIVE_INT_TYPE Queue::getQueueSize(void) const {
        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return (U32) attr.mq_maxmsg;
    }

    NATIVE_INT_TYPE Queue::getMsgSize(void) const {
        QueueHandle* queueHandle = (QueueHandle*) this->m_handle;
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return (U32) attr.mq_msgsize;
    }

}

