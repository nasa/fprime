// ======================================================================
// \title Os/Generic/PriorityQueue.hpp
// \brief priority queue implementation definitions for Os::Queue
// ======================================================================
#include "Os/Condition.hpp"
#include "Os/Generic/Types/MaxHeap.hpp"
#include "Os/Mutex.hpp"
#include "Os/Queue.hpp"
#ifndef OS_GENERIC_PRIORITYQUEUE_HPP
#define OS_GENERIC_PRIORITYQUEUE_HPP

namespace Os {
namespace Generic {

//! \brief critical data stored for priority queue
//!
//! The priority queue has two essential data structures: a block of unordered memory storing message data and size. The
//! queue also stores a circular list of indices into that memory tracking which slots are free and which are taken.
//! These indices are ordered by a max heap data structure projecting priority on to the otherwise unordered data. Both
//! the data region and index list have queue depth number of entries.
struct PriorityQueueHandle : public QueueHandle {
    Types::MaxHeap m_heap;            //!< MaxHeap data store for tracking priority
    U8* m_data = nullptr;             //!< Pointer to data allocation
    FwSizeType* m_indices = nullptr;  //!< List of indices into data
    FwSizeType* m_sizes = nullptr;    //!< Size store for each method
    FwSizeType m_depth = 0;           //!< Depth of the queue
    FwSizeType m_startIndex = 0;      //!< Start index of the circular data structure
    FwSizeType m_stopIndex = 0;       //!< End index of the circular data structure
    FwSizeType m_maxSize = 0;         //!< Maximum size allowed of a message
    FwSizeType m_highMark = 0;        //!< Message count high water mark
    Os::Mutex m_data_lock;            //!< Lock against data manipulation
    Os::ConditionVariable m_full;     //!< Queue full condition variable to support blocking
    Os::ConditionVariable m_empty;    //!< Queue empty condition variable to support blocking

    //!\brief find an available index to store data from the list
    FwSizeType find_index();

    //!\brief return index to the circular data structure
    //!\param index: index to return to the list
    void return_index(FwSizeType index);

    //!\brief store data into a set index in the data store
    void store_data(FwSizeType index, const U8* source, FwSizeType size);

    //!\brief load data from a set index in the data store
    void load_data(FwSizeType index, U8* destination, FwSizeType capacity);
};
//! \brief generic priority queue implementation
//!
//! A generic implementation of a priority queue to support the Os::QueueInterface. This queue uses OSAL mutexes,
//! and condition variables to provide for a task-safe blocking queue implementation. Data is stored in heap memory.
//!
//! \warning allocates memory on the heap
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
    PriorityQueue& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each.
    //!
    //! \warning allocates memory on the heap
    //!
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: status of the creation
    Status create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) override;

    //! \brief send a message into the queue
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full.
    //!
    //! \warning It is invalid to send a null buffer
    //! \warning This method will block if the queue is full and blockType is set to BLOCKING
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
    //! \warning It is invalid to send a null buffer
    //! \warning This method will block if the queue is full and blockType is set to BLOCKING
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

    QueueHandle* getHandle() override;

    PriorityQueueHandle m_handle;
};
}  // namespace Generic
}  // namespace Os

#endif  // OS_GENERIC_PRIORITYQUEUE_HPP
