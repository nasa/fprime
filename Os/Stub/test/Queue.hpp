
#ifndef OS_STUB_TEST_QUEUE_HPP
#define OS_STUB_TEST_QUEUE_HPP
#include "Os/Queue.hpp"
#include <queue>
#include <deque>


namespace Os {
namespace Stub {
namespace Queue {
namespace Test {
constexpr FwSizeType STUB_QUEUE_TEST_MESSAGE_MAX_SIZE = 1024;

//! Data that supports the stubbed File implementation.
//!
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        CREATE_FN,
        SEND_FN,
        RECEIVE_FN,
        MESSAGES_FN,
        HIGH_WATER_FN,
        HANDLE_FN
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;

    Os::QueueInterface::Status createStatus = Os::QueueInterface::Status::OP_OK;
    Os::QueueInterface::Status sendStatus = Os::QueueInterface::Status::OP_OK;
    Os::QueueInterface::Status receiveStatus = Os::QueueInterface::Status::OP_OK;

    FwSignedSizeType messages = -1;
    FwSignedSizeType highWaterMark = -1;
    QueueHandle* handle = nullptr;

    // Received variables
    Os::QueueString name;
    FwSizeType depth;
    FwSizeType size;
    FwSizeType capacity;
    U8* buffer;
    FwQueuePriorityType priority;
    QueueInterface::BlockingType blockType;

    // Singleton data
    static StaticData data;
};


struct InjectableStlQueueHandle : public QueueHandle {
    //! \brief message type
    struct Message {
        U8 data[STUB_QUEUE_TEST_MESSAGE_MAX_SIZE];
        FwQueuePriorityType priority;
        FwSizeType size;
        //! \brief comparison utility for messages
        struct LessMessage {
            bool operator()(const Message& a, const Message& b) {
                return std::greater<FwQueuePriorityType>()(a.priority, b.priority);
            }
        };
    };
    InjectableStlQueueHandle();
    ~InjectableStlQueueHandle();

    std::priority_queue<Message, std::deque<Message>, Message::LessMessage>& m_storage;
    FwSizeType m_high_water;
    FwSizeType m_max_depth;
};

//! \brief standard library powered queue implementation with injectable statuses
class InjectableStlQueue : public QueueInterface {
  public:
    //! \brief default queue interface constructor
    InjectableStlQueue();

    //! \brief default queue destructor
    virtual ~InjectableStlQueue();

    //! \brief copy constructor is forbidden
    InjectableStlQueue(const QueueInterface& other) = delete;

    //! \brief copy constructor is forbidden
    InjectableStlQueue(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    InjectableStlQueue& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each.
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

    QueueHandle* getHandle() override;

    InjectableStlQueueHandle m_handle;
};

}  // namespace Test
}  // namespace Queue
}  // namespace Stub
}  // namespace Os

#endif  // OS_STUB_TEST_QUEUE_HPP
