// ======================================================================
// \title Os/Queue.hpp
// \brief common function definitions for Os::Queue
// ======================================================================
#ifndef Os_Queue_hpp_
#define Os_Queue_hpp_

#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/Os.hpp>
#include <Os/QueueString.hpp>
#include <Os/Mutex.hpp>
namespace Os {
// Forward declaration for registry
class QueueRegistry;

//! \brief QueueHandle parent class
class QueueHandle {};

//! \brief base queue interface
//!
//! Queues are used internally to fprime in order to support the messaging between components. The
//! QueueInterface is used to abstract away from the standard OS-based queue, allowing F prime support
//! multiple OSes in a consistent way.
//!
class QueueInterface {
  public:
    //! \brief status returned from the queue send function
    enum Status {
        OP_OK,             //!<  message sent/received okay
        ALREADY_CREATED,   //!<  creating an already created queue
        EMPTY,             //!<  If non-blocking, all the messages have been drained.
        UNINITIALIZED,     //!<  Queue wasn't initialized successfully
        SIZE_MISMATCH,     //!<  attempted to send or receive with buffer too large, too small
        SEND_ERROR,        //!<  message send error
        RECEIVE_ERROR,     //!<  message receive error
        INVALID_PRIORITY,  //!<  invalid priority requested
        FULL,              //!< queue was full when attempting to send a message
        UNKNOWN_ERROR      //!<  Unexpected error; can't match with returns
    };

    //! \brief message type
    enum BlockingType {
        BLOCKING,    //!< Message will block until space is available
        NONBLOCKING  //!< Message will return with status when space is unavailable
    };

    //! \brief default queue interface constructor
    QueueInterface() = default;

    //! \brief default queue destructor
    virtual ~QueueInterface() = default;

    //! \brief copy constructor is forbidden
    QueueInterface(const QueueInterface& other) = delete;

    //! \brief copy constructor is forbidden
    QueueInterface(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    virtual QueueInterface& operator=(const QueueInterface& other) = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each. Resource
    //! allocation is dependent on the underlying implementation and users should assume that resource allocation is
    //! possible.
    //!
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: status of the creation
    virtual Status create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) = 0;

    //! \brief send a message into the queue
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full.
    //!
    //! It is invalid to send a null buffer.
    //! This method will block if the queue is full and blockType is set to BLOCKING
    //!
    //! \param buffer: message data
    //! \param size: size of message data
    //! \param priority: priority of the message
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return: status of the send
    virtual Status send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, BlockingType blockType) = 0;

    //! \brief receive a message from the queue
    //!
    //! Receive a message from the queue, providing the message destination, capacity, priority, and blocking type.
    //! When `blockType` is set to BLOCKING, this call will block on queue empty. Otherwise, this will return an
    //! error status on queue empty. Actual size received and priority of message is set on success status.
    //!
    //! It is invalid to send a null buffer.
    //! This method will block if the queue is empty and blockType is set to BLOCKING
    //!
    //! \param destination: destination for message data
    //! \param capacity: maximum size of message data
    //! \param blockType: BLOCKING to wait for message or NONBLOCKING to return error when queue is empty
    //! \param actualSize: (output) actual size of message read
    //! \param priority: (output) priority of message read
    //! \return: status of the send
    virtual Status receive(U8* destination,
                           FwSizeType capacity,
                           BlockingType blockType,
                           FwSizeType& actualSize,
                           FwQueuePriorityType& priority) = 0;

    //! \brief get number of messages available
    //!
    //! Returns the number of messages currently available in the queue.
    //!
    //! \return number of messages available
    virtual FwSizeType getMessagesAvailable() const = 0;

    //! \brief get maximum messages stored at any given time
    //!
    //! Returns the maximum number of messages in this queue at any given time. This is the high-water mark for this
    //! queue.
    //! \return queue message high-water mark
    virtual FwSizeType getMessageHighWaterMark() const = 0;

    //! \brief return the underlying queue handle (implementation specific)
    //! \return internal task handle representation
    virtual QueueHandle* getHandle() = 0;

    //! \brief provide a pointer to a queue delegate object
    //!
    //! This function must return a pointer to a `QueueInterface` object that contains the real implementation of the
    //! queue functions as defined by the implementor.  This function must do several things to be considered correctly
    //! implemented:
    //!
    //! 1. Assert that the supplied memory is non-null. e.g `FW_ASSERT(aligned_placement_new_memory != NULL);`
    //! 2. Assert that their implementation fits within FW_HANDLE_MAX_SIZE.
    //!    e.g. `static_assert(sizeof(PosixQueueImplementation) <= sizeof Os::Queue::m_handle_storage,
    //!        "FW_HANDLE_MAX_SIZE to small");`
    //! 3. Assert that their implementation aligns within FW_HANDLE_ALIGNMENT.
    //!    e.g. `static_assert((FW_HANDLE_ALIGNMENT % alignof(PosixQueueImplementation)) == 0, "Bad handle alignment");`
    //! 4. Placement new their implementation into `aligned_placement_new_memory`
    //!    e.g. `TaskInterface* interface = new (aligned_placement_new_memory) PosixQueueImplementation;`
    //! 5. Return the result of the placement new
    //!    e.g. `return interface;`
    //!
    //! \return result of placement new, must be equivalent to `aligned_placement_new_memory`
    //!
    static QueueInterface* getDelegate(QueueHandleStorage& aligned_placement_new_memory);
};

class Queue final : public QueueInterface {
  public:
    //! \brief queue constructor
    Queue();

    //! \brief default queue destructor
    virtual ~Queue();

    //! \brief copy constructor is forbidden
    Queue(const Queue& other) = delete;

    //! \brief copy constructor is forbidden
    Queue(const Queue* other) = delete;

    //! \brief assignment operator is forbidden
    QueueInterface& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage through delegate
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each. This method
    //! delegates to the underlying implementation. Resource allocation is dependent on the underlying implementation
    //! and users should assume that resource allocation is possible.
    //!
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: status of the creation
    Status create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) override;

    //! \brief send a message into the queue through delegate
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full. This method delegates to the underlying implementation.
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

    //! \brief receive a message from the queue through delegate
    //!
    //! Receive a message from the queue, providing the message destination, capacity, priority, and blocking type.
    //! When `blockType` is set to BLOCKING, this call will block on queue empty. Otherwise, this will return an
    //! error status on queue empty. Actual size received and priority of message is set on success status. This method
    //! delegates to the underlying implementation.
    //!
    //! \warning It is invalid to send a null buffer.
    //! \warning This method will block if the queue is empty and blockType is set to BLOCKING
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
    //! Returns the number of messages currently available in the queue. This method delegates to the underlying
    //! implementation.
    //!
    //! \return number of messages available
    FwSizeType getMessagesAvailable() const override;

    //! \brief get maximum messages stored at any given time through delegate
    //!
    //! Returns the maximum number of messages in this queue at any given time. This is the high-water mark for
    //! this queue.
    //! \return queue message high-water mark
    FwSizeType getMessageHighWaterMark() const override;

    //! \brief return the underlying queue handle (implementation specific). Delegates to implementation.
    //! \return internal task handle representation
    QueueHandle* getHandle() override;

    //! \brief send a message to a queue
    //!
    //! Send a message to a queue with the given priority and block type. See: QueueInterface::send
    //!
    //! \warning This method will block if the queue is full and blockType is set to BLOCKING
    //!
    //! \param message: reference to serialize buffer storing message
    //! \param priority: priority of the message
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return status of the send
    Status send(const Fw::SerializeBufferBase& message, FwQueuePriorityType priority, BlockingType blockType);

    //! \brief receive a message from a queue
    //!
    //! Receive a message from a queue with the given block type. See: QueueInterface::receive. Note: this will entirely
    //! overwrite the buffer.
    //!
    //! \warning This method will block if the queue is full and blockType is set to BLOCKING
    //!
    //! \param destination: reference to serialize buffer for storing message
    //! \param priority: (output) priority of the message
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return status of the send
    Status receive(Fw::SerializeBufferBase& destination, BlockingType blockType, FwQueuePriorityType& priority);

    //! \brief get the queue's depth in messages
    FwSizeType getDepth() const;

    //! \brief get the queue's message maximum size
    FwSizeType getMessageSize() const;

    //! \brief get the queue's name
    const QueueString& getName() const;

    //! \brief get number of queues system-wide
    static FwSizeType getNumQueues();

    //! \brief get static mutex
    static Os::Mutex& getStaticMutex();

  private:
    QueueString m_name;                           //!< queue name
    FwSizeType m_depth;                           //!< Queue depth
    FwSizeType m_size;                            //!< Maximum message size
    static Os::Mutex s_countLock;                 //!< Lock the count
    static FwSizeType s_queueCount;               //!< Count of the number of queues

#if FW_QUEUE_REGISTRATION
  public:
    //! \brief set QueueRegistry for tracking queues
    //!
    //! \param registry: registry to set
    static void setRegistry(QueueRegistry* registry);

  private:
    static QueueRegistry* s_queueRegistry;  //!< Queue registry store
#endif

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) QueueHandleStorage m_handle_storage;  //!< Storage for aligned handle
    QueueInterface& m_delegate;                                        //!< Delegate for the real implementation
};
//! \brief queue registry interface
//!
//! The QueueRegistry is used to track queues in the system. There is intended to be a single, global, queue registry
//! across the system. It is used to track queues and will receive a callback on the creation of each queue.
class QueueRegistry {
  public:
    //! Default QueueRegistry
    QueueRegistry() = default;
    //! Default ~QueueRegistry
    virtual ~QueueRegistry() = default;

    //! \brief queue registry callback
    //!
    //! Register the queue with this queue registry. Must be implemented by QueueRegistry implementations.
    //!
    //! \param queue: queue being registered
    virtual void registerQueue(Queue* queue) = 0;  //!< method called by queue init() methods to register a new queue
};
}  // namespace Os
#endif
