#include <Os/LocklessQueue.hpp>
#include <Fw/Types/Assert.hpp>

#include <fcntl.h>
#include <cstring>
#include <new>

#define CAS(a_ptr, a_oldVal, a_newVal) __sync_bool_compare_and_swap(a_ptr, a_oldVal, a_newVal)

namespace Os {

    LocklessQueue::LocklessQueue(const NATIVE_INT_TYPE maxmsg,
            const NATIVE_INT_TYPE msgsize) {
#ifndef BUILD_DARWIN
        m_attr.mq_flags = O_NONBLOCK;
        m_attr.mq_maxmsg = maxmsg;
        m_attr.mq_msgsize = msgsize;
        m_attr.mq_curmsgs = 0;
#endif
        // Must have at least 2 messages in the queue
        FW_ASSERT(maxmsg >= 2, maxmsg);

        m_index = new(std::nothrow) QueueNode[maxmsg]; // Allocate an index entry for each msg
        m_data = new(std::nothrow) U8[maxmsg * msgsize];  // Allocate data for each msg
        FW_ASSERT(m_index != nullptr);
        FW_ASSERT(m_data != nullptr);

        for (int i = 0, j = 1; i < maxmsg; i++, j++) {
            m_index[i].data = &m_data[i * msgsize]; // Assign the data pointer of index to top that msg's buffer

            if (j < maxmsg) {           // If we aren't processing the last item
                m_index[i].next = &m_index[j]; // Chain this item to the next one
            } else {
                m_index[i].next = nullptr;         // Initialize to NULL otherwise
            }
        }

        // Assign each of the pointers to the first index element
        // This one is held in a "dummy" position for now
        // It will be cleaned up by the producer
        m_first = &m_index[0];
        m_last = &m_index[0];
        m_last->next = nullptr;

        // Assign the head of the free list to the second element
        m_free_head = &m_index[1];
    }

    LocklessQueue::~LocklessQueue() {
        delete[] m_index;
        delete[] m_data;
    }

#ifndef BUILD_DARWIN
    void LocklessQueue::GetAttr(mq_attr & attr) {
        memcpy(&attr, &m_attr, sizeof(mq_attr));
    }
#endif

    void LocklessQueue::PushFree(QueueNode * my_node) {
        QueueNode * old_free_head;

        FW_ASSERT(my_node != nullptr);

        // CAS the node into the free list
        do {
            old_free_head = m_free_head;
            my_node->next = old_free_head;
        } while (!CAS(&m_free_head, old_free_head, my_node));
    }

    bool LocklessQueue::PopFree(QueueNode ** free_node) {
        QueueNode * my_node;

        // CAS a buffer from the free list
        do {
            my_node = m_free_head;

            if (nullptr == my_node) {
                return false;
            }

        } while (!CAS(&m_free_head, my_node, my_node->next));

        (*free_node) = my_node;

        return true;
    }

    Queue::QueueStatus LocklessQueue::Send(const U8 * buffer,
            NATIVE_INT_TYPE size) {

        QueueNode * my_node;
        QueueNode * old_last;

#ifndef BUILD_DARWIN
        // Check that the new message will fit in our buffers
        if (size > m_attr.mq_msgsize) {
            return Queue::QUEUE_SIZE_MISMATCH;
        }
#endif

        if (!PopFree(&my_node)) {
            return Queue::QUEUE_FULL;
        }

        // Copy the data into the buffer
        memcpy(my_node->data, buffer, static_cast<size_t>(size));
        my_node->size = size;
        my_node->next = nullptr;

        // Publish the node
        // The m_last pointer is moved before the item is published
        // All concurrent writers will argue over their order using CAS
        do {
            old_last = m_last;
        } while (!CAS(&m_last, old_last, my_node));

        old_last->next = my_node;

        return Queue::QUEUE_OK;
    }

    // ONLY ONE RECEIVER AT A TIME!!!!
    Queue::QueueStatus LocklessQueue::Receive(U8 * buffer,
            NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE & size) {

        QueueNode * my_node;
        QueueNode * old_node;

#ifndef BUILD_DARWIN
        if (capacity < m_attr.mq_msgsize) {
            return Queue::QUEUE_SIZE_MISMATCH;
        }
#endif

        if (m_first == m_last) {
            return Queue::QUEUE_NO_MORE_MSGS;
        }

        old_node = m_first;
        my_node = m_first->next;

        if (my_node == nullptr) {
            // We may not be fully linked yet, even though "last" has moved
            return Queue::QUEUE_NO_MORE_MSGS;
        }

        m_first = m_first->next;
        PushFree(old_node);

        // Copy the data from the buffer
        memcpy(buffer, my_node->data, static_cast<size_t>(my_node->size));
        size = my_node->size;

        return Queue::QUEUE_OK;
    }

}
