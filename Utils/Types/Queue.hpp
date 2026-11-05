/*
 * Queue.hpp:
 *
 * FIFO/LIFO queue of fixed size messages. For use generally where non-concurrent, non-OS backed based queues are
 * necessary. Message size is defined at construction time and all messages enqueued and dequeued must be of that fixed
 * size. Wraps circular buffer to perform actual storage of messages. This implementation is not thread safe and the
 * expectation is that the user will wrap it in concurrency constructs where necessary.
 *
 *  Created on: July 5th, 2022
 *      Author: lestarch
 *
 */
#ifndef _UTILS_TYPES_QUEUE_HPP
#define _UTILS_TYPES_QUEUE_HPP
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Utils/Types/CircularBuffer.hpp>

namespace Types {

/**
 * \brief Queue ordering mode
 */
enum QueueMode {
    QUEUE_FIFO,  //!< First-In-First-Out: dequeue from front
    QUEUE_LIFO   //!< Last-In-First-Out: dequeue from back
};

/**
 * \brief Queue overflow behavior mode
 */
enum QueueOverflowMode {
    QUEUE_DROP_NEWEST,  //!< Drop the newest (incoming) message on overflow
    QUEUE_DROP_OLDEST   //!< Drop the oldest (front) message on overflow
};

class Queue {
  public:
    /**
     * \brief constructs an uninitialized queue
     */
    Queue();

    /**
     * \brief setup the queue object to setup storage
     *
     * The queue must be configured before use to setup storage parameters. This function supplies those parameters
     * including depth, message size, queue mode, and overflow mode.  Storage size must be greater than or equal to the
     * depth x message size.
     *
     * \param storage: storage memory allocation
     * \param storage_size: size of the provided allocation
     * \param depth: depth of the queue
     * \param message_size: size of individual messages
     * \param mode: queue ordering mode (FIFO or LIFO), defaults to FIFO
     * \param overflow_mode: overflow handling mode, defaults to DROP_NEWEST
     */
    void setup(U8* const storage,
               const FwSizeType storage_size,
               const FwSizeType depth,
               const FwSizeType message_size,
               const QueueMode mode = QUEUE_FIFO,
               const QueueOverflowMode overflow_mode = QUEUE_DROP_NEWEST);

    /**
     * \brief pushes a fixed-size message onto the queue
     *
     * Pushes a fixed-size message onto the queue. This performs a copy of the data onto the queue so the user is free
     * to dispose the message data as soon as the call returns. Note: message is required to be of the size message_size
     * as defined by the construction of the queue. Size is provided as a safety check to ensure the sent size is
     * consistent with the expected size of the queue.
     *
     * When the queue is full, behavior depends on the overflow mode:
     * - DROP_NEWEST: Returns FW_SERIALIZE_NO_ROOM_LEFT without modifying the queue
     * - DROP_OLDEST: Removes the oldest message and adds the new one, returns FW_SERIALIZE_DISCARDED_EXISTING
     *
     * \param message: message of size m_message_size to enqueue
     * \param size: size of the message being sent. Must be equivalent to queue's message size.
     * \return: Fw::SERIALIZE_OK on success, FW_SERIALIZE_NO_ROOM_LEFT when full with DROP_NEWEST mode,
     * FW_SERIALIZE_DISCARDED_EXISTING when full with DROP_OLDEST mode
     */
    Fw::SerializeStatus enqueue(const U8* const message, const FwSizeType size);

    /**
     * \brief pushes a serializable object onto the queue
     *
     * Pushes a serializable object onto the queue using F Prime serialization into the queue's
     * fixed-size message slot. The object's serialized size must fit within the queue's message
     * size. Overflow behavior is identical to the raw-byte enqueue.
     *
     * \param message: serializable object to enqueue
     * \return: Fw::FW_SERIALIZE_OK on success, FW_SERIALIZE_NO_ROOM_LEFT when full with DROP_NEWEST mode,
     * FW_SERIALIZE_DISCARDED_EXISTING when full with DROP_OLDEST mode
     */
    Fw::SerializeStatus enqueue(const Fw::Serializable& message);

    /**
     * \brief pushes linear buffer contents onto the queue
     *
     * Pushes the contents of a linear buffer (length token followed by data) onto the queue using
     * F Prime serialization into the queue's fixed-size message slot. The buffer's serialized size
     * must fit within the queue's message size. Overflow behavior is identical to the raw-byte
     * enqueue.
     *
     * \param message: linear buffer whose contents to enqueue
     * \return: Fw::FW_SERIALIZE_OK on success, FW_SERIALIZE_NO_ROOM_LEFT when full with DROP_NEWEST mode,
     * FW_SERIALIZE_DISCARDED_EXISTING when full with DROP_OLDEST mode
     */
    Fw::SerializeStatus enqueue(const Fw::LinearBufferBase& message);

    /**
     * \brief pops a fixed-size message off the queue
     *
     * Pops a fixed-size message off the queue. This performs a copy of the data into the provided message
     * buffer. Note: message is required to be of the size message_size as defined by the construction of the queue. The
     * size must be greater or equal to message size, although only message size bytes will be used.
     *
     * Dequeue location depends on the queue mode:
     * - FIFO: Removes and returns the oldest (front) message
     * - LIFO: Removes and returns the newest (back) message
     *
     * This will return a non-Fw::SERIALIZE_OK status when the queue is empty.
     *
     * \param message: message of size m_message_size to dequeue
     * \param size: size of the buffer being supplied.
     * \return: Fw::SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus dequeue(U8* const message, const FwSizeType size);

    /**
     * \brief pops a serializable object off the queue
     *
     * Pops a serializable object off the queue by deserializing the queue's fixed-size message
     * slot into the supplied object. Dequeue location depends on the queue mode (FIFO or LIFO)
     * exactly as the raw-byte dequeue.
     *
     * \param message: serializable object to fill from the dequeued message
     * \return: Fw::FW_SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus dequeue(Fw::Serializable& message);

    /**
     * \brief pops linear buffer contents off the queue
     *
     * Pops linear buffer contents off the queue by deserializing the queue's fixed-size message
     * slot into the supplied buffer. Dequeue location depends on the queue mode (FIFO or LIFO)
     * exactly as the raw-byte dequeue.
     *
     * \param message: linear buffer to fill from the dequeued message
     * \return: Fw::FW_SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus dequeue(Fw::LinearBufferBase& message);

    /**
     * \brief removes and returns the oldest (front) message regardless of queue mode
     *
     * Unlike dequeue(), which respects the queue mode (FIFO vs LIFO), this method always
     * removes from the front of the queue.  This is needed when callers must capture the
     * oldest entry before it is discarded (e.g. returning buffer ownership on DROP_OLDEST
     * overflow).
     *
     * \param message: buffer of at least m_message_size bytes to receive the oldest message
     * \param size: size of the buffer being supplied
     * \return: Fw::SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus popFront(U8* const message, const FwSizeType size);

    /**
     * \brief removes and returns the oldest (front) message into a serializable object
     *
     * Serializable-object equivalent of the raw-byte popFront: always removes from the front of
     * the queue regardless of queue mode.
     *
     * \param message: serializable object to fill from the oldest message
     * \return: Fw::FW_SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus popFront(Fw::Serializable& message);

    /**
     * \brief removes and returns the oldest (front) message into a linear buffer
     *
     * Linear-buffer equivalent of the raw-byte popFront: always removes from the front of the
     * queue regardless of queue mode.
     *
     * \param message: linear buffer to fill from the oldest message
     * \return: Fw::FW_SERIALIZE_OK on success, something else on failure
     */
    Fw::SerializeStatus popFront(Fw::LinearBufferBase& message);

    /**
     * Return the largest tracked allocated size
     */
    FwSizeType get_high_water_mark() const;

    /**
     * Clear tracking of the largest allocated size
     */
    void clear_high_water_mark();

    FwSizeType getQueueSize() const;

  private:
    //! Common implementation for the serializable-object enqueue overloads
    template <typename T>
    Fw::SerializeStatus enqueue_impl(const T& message);

    //! Common implementation for the serializable-object dequeue overloads
    template <typename T>
    Fw::SerializeStatus dequeue_impl(T& message);

    //! Common implementation for the serializable-object popFront overloads
    template <typename T>
    Fw::SerializeStatus popFront_impl(T& message);

    CircularBuffer m_internal;
    FwSizeType m_message_size;
    QueueMode m_mode;
    QueueOverflowMode m_overflow_mode;
};
}  // namespace Types
#endif  // _UTILS_TYPES_QUEUE_HPP
