#include <Fw/Types/Assert.hpp>
#include <Os/Queue.hpp>
#include <Os/IPCQueue.hpp>

#ifdef TGT_OS_TYPE_VXWORKS
    #include <vxWorks.h>
#endif

#ifdef TGT_OS_TYPE_LINUX
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include <mqueue.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include <pthread.h>
#include <new>

#define IPC_QUEUE_TIMEOUT_SEC (1)

namespace Os {

    class QueueHandle {
        public:
        QueueHandle(mqd_t m_handle) {
            this->handle = m_handle;
        }
        ~QueueHandle() {
            // Destroy the handle:
            if (-1 != this->handle) {
                (void) mq_close(this->handle);
            }
        }
        mqd_t handle;
    };

    IPCQueue::IPCQueue() : Queue() {
    }

    Queue::QueueStatus IPCQueue::create(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {

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

        /* NOTE(mereweth) - O_BLOCK is the default; we use timedsend and
         * timedreceive below if QUEUE_NONBLOCKING is specified
         *
         */
        handle = mq_open(this->m_name.toChar(), O_RDWR | O_CREAT | O_EXCL, 0666, &att);

        // If queue already exists, then unlink it and try again.
        if (-1 ==  handle) {
            switch (errno) {
                case EEXIST:
                    (void)mq_unlink(this->m_name.toChar());
                    break;
                default:
                    return QUEUE_UNINITIALIZED;
            }

            handle = mq_open(this->m_name.toChar(), O_RDWR | O_CREAT | O_EXCL, 0666, &att);

            if (-1 == handle) {
                return QUEUE_UNINITIALIZED;
            }
        }

        // Set up queue handle:
        QueueHandle* queueHandle = new(std::nothrow) QueueHandle(handle);
        if (nullptr == queueHandle) {
          return QUEUE_UNINITIALIZED;
        }
        this->m_handle = reinterpret_cast<POINTER_CAST>(queueHandle);

        Queue::s_numQueues++;

        return QUEUE_OK;
    }

    IPCQueue::~IPCQueue() {
        // Clean up the queue handle:
        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        if (nullptr != queueHandle) {
          delete queueHandle;
        }
        this->m_handle = reinterpret_cast<POINTER_CAST>(nullptr); // important so base Queue class doesn't free it
        (void) mq_unlink(this->m_name.toChar());
    }

    Queue::QueueStatus IPCQueue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {

        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        mqd_t handle = queueHandle->handle;

        if (-1 == handle) {
            return QUEUE_UNINITIALIZED;
        }

        if (nullptr == buffer) {
            return QUEUE_EMPTY_BUFFER;
        }

        bool keepTrying = true;
        while (keepTrying) {
            struct timeval now;
            gettimeofday(&now,nullptr);
            struct timespec wait;
            wait.tv_sec = now.tv_sec;
            wait.tv_nsec = now.tv_usec * 1000;

            if (block == QUEUE_BLOCKING) {
                wait.tv_sec += IPC_QUEUE_TIMEOUT_SEC;
            }

            NATIVE_INT_TYPE stat = mq_timedsend(
                handle,
                reinterpret_cast<const char*>(buffer),
                static_cast<size_t>(size),
                static_cast<unsigned int>(priority),
                &wait);

            if (-1 == stat) {
                switch (errno) {
                    case EINTR:
                        continue;
                    case EMSGSIZE:
                        return QUEUE_SIZE_MISMATCH;
                    case EINVAL:
                        return QUEUE_INVALID_PRIORITY;
                    case ETIMEDOUT:
                        if (block == QUEUE_NONBLOCKING) {
                            // no more messages. If we are
                            // non-blocking, return
                            return QUEUE_FULL;
                        } else {
                            // TODO(mereweth) - multiprocess signalling necessary?
                            // Go to sleep until we receive a signal that something was taken off the queue
                            continue;
                        }
                    default:
                        return QUEUE_UNKNOWN_ERROR;
                }
            } else {
                keepTrying=false;
                // TODO(mereweth) - multiprocess signalling necessary?
                // Wake up a thread that might be waiting on the other end of the queue:
            }
        }

        return QUEUE_OK;
    }

    Queue::QueueStatus IPCQueue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {

        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        mqd_t handle = queueHandle->handle;

        if (-1 == handle) {
            return QUEUE_UNINITIALIZED;
        }

        ssize_t size;
        bool notFinished = true;
        while (notFinished) {
            struct timeval now;
            gettimeofday(&now,nullptr);
            struct timespec wait;
            wait.tv_sec = now.tv_sec;
            wait.tv_nsec = now.tv_usec * 1000;

            if (block == QUEUE_BLOCKING) {
                wait.tv_sec += IPC_QUEUE_TIMEOUT_SEC;
            }
            size = mq_timedreceive(handle, reinterpret_cast<char*>(buffer), static_cast<size_t>(capacity),
#ifdef TGT_OS_TYPE_VXWORKS
                        reinterpret_cast<int*>(&priority), &wait);
#else
                        reinterpret_cast<unsigned int*>(&priority), &wait);
#endif

            if (-1 == size) { // error
                switch (errno) {
                    case EINTR:
                        continue;
                    case EMSGSIZE:
                        return QUEUE_SIZE_MISMATCH;
                    case ETIMEDOUT:
                        if (block == QUEUE_NONBLOCKING) {
                            // no more messages. If we are
                            // non-blocking, return
                            return QUEUE_NO_MORE_MSGS;
                        } else {
                            // TODO(mereweth) - multiprocess signalling necessary?
                            // Go to sleep until we receive a signal that something was put on the queue:
                            continue;
                        }
                        break;
                    default:
                        return QUEUE_UNKNOWN_ERROR;
                }
            }
            else {
                notFinished = false;
                // TODO(mereweth) - multiprocess signalling necessary?
                // Wake up a thread that might be waiting on the other end of the queue
            }
        }

        actualSize = static_cast<NATIVE_INT_TYPE>(size);
        return QUEUE_OK;
    }

    NATIVE_INT_TYPE IPCQueue::getNumMsgs() const {
        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return static_cast<NATIVE_INT_TYPE>(attr.mq_curmsgs);
    }

    NATIVE_INT_TYPE IPCQueue::getMaxMsgs() const {
        //FW_ASSERT(0);
        return 0;
    }

    NATIVE_INT_TYPE IPCQueue::getQueueSize() const {
        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return static_cast<NATIVE_INT_TYPE>(attr.mq_maxmsg);
    }

    NATIVE_INT_TYPE IPCQueue::getMsgSize() const {
        QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
        mqd_t handle = queueHandle->handle;

        struct mq_attr attr;
        int status = mq_getattr(handle, &attr);
        FW_ASSERT(status == 0);
        return static_cast<NATIVE_INT_TYPE>(attr.mq_msgsize);
    }

}
