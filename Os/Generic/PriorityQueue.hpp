//
// Created by Michael Starch on 9/17/24.
//
#include "Os/Queue.hpp"
#include "Os/Mutex.hpp"
#include "Os/Condition.hpp"
#include "Os/Generic/Types/MaxHeap.hpp"
#ifndef OS_GENERIC_PRIORITYQUEUE_HPP
#define OS_GENERIC_PRIORITYQUEUE_HPP

namespace Os {
namespace Generic {

struct Metadata {

};

struct PriorityQueueHandle : public QueueHandle {
    Types::MaxHeap m_heap;
    U8* m_data = nullptr;
    FwSizeType* m_indices = nullptr;
    FwSizeType* m_sizes = nullptr;
    FwSizeType m_depth = 0;
    FwSizeType m_startIndex = 0;
    FwSizeType m_stopIndex = 0;
    FwSizeType m_maxSize = 0;
    FwSizeType m_hwm = 0;
    Os::Mutex m_data_lock;
    Os::ConditionVariable m_full;
    Os::ConditionVariable m_empty;

    FwSizeType find_index();

    void return_index(FwSizeType index);

    void store_data(FwSizeType index, const U8* source, FwSizeType size);

    void load_data(FwSizeType index, U8* destination, FwSizeType capacity);
};

class PriorityQueue : public Os::QueueInterface {
  public:
    //! \brief default queue interface constructor
    PriorityQueue() = default;

    //! \brief default queue destructor
    virtual ~PriorityQueue();

    //! \brief copy constructor is forbidden
    PriorityQueue(const QueueInterface& other) = delete;

    //! \brief copy constructor is forbidden
    PriorityQueue(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    PriorityQueue& operator=(const QueueInterface& other) = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each.
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: stauts of the creation
    Status create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) override;

    //! \brief send a message into the queue
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full.
    //!
    //! \param buffer: message data
    //! \param size: size of message data
    //! \param priority: priority of the message
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return: status of the send
    Status send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, BlockingType blockType) override;

    //! \brief receive a message from the queue
    //!
    //! Receive a message from the queue, providing the message destination, capacity, priority, and blocking type.
    //! When `blockType` is set to BLOCKING, this call will block on queue empty. Otherwise, this will return an
    //! error status on queue empty. Actual size received and priority of message is set on success status.
    //!
    //! \param destination: destination for message data
    //! \param capacity: maximum size of message data
    //! \param blockType: BLOCKING to wait for message or NONBLOCKING to return error when queue is empty
    //! \param actualSize: (output) actual size of message read
    //! \param priority: (output) priority of message read
    //! \return: status of the send
    Status receive(U8* destination,
                   FwSizeType capacity,
                   BlockingType blockType,
                   FwSizeType& actualSize,
                   FwQueuePriorityType& priority) override;

    //! \brief get number of messages available
    //!
    //! \return number of messages available
    FwSizeType getMessagesAvailable() const override;

    //! \brief get maximum messages stored at any given time
    //!
    //! Returns the maximum number of messages in this queue at any given time. This is the high-water mark for this
    //! queue.
    //! \return queue message high-water mark
    FwSizeType getMessageHighWaterMark() const override;

    PriorityQueueHandle m_handle;
};
}  // namespace Generic
}  // namespace Os

#endif  // OS_GENERIC_PRIORITYQUEUE_HPP
