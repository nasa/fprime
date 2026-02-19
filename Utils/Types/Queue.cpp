/*
 * Queue.cpp:
 *
 * Implementation of the queue data type.
 *
 *  Created on: July 5th, 2022
 *      Author: lestarch
 *
 */
#include "Queue.hpp"
#include <Fw/Types/Assert.hpp>

namespace Types {

Queue::Queue() : m_internal(), m_message_size(0), m_mode(QUEUE_FIFO), m_overflow_mode(QUEUE_DROP_NEWEST) {}

void Queue::setup(U8* const storage,
                  const FwSizeType storage_size,
                  const FwSizeType depth,
                  const FwSizeType message_size,
                  const QueueMode mode,
                  const QueueOverflowMode overflow_mode) {
    // Ensure that enough storage was supplied
    const FwSizeType total_needed_size = depth * message_size;
    FW_ASSERT(storage_size >= total_needed_size, static_cast<FwAssertArgType>(storage_size),
              static_cast<FwAssertArgType>(depth), static_cast<FwAssertArgType>(message_size));
    FW_ASSERT(mode == QUEUE_FIFO || mode == QUEUE_LIFO, static_cast<FwAssertArgType>(mode));
    FW_ASSERT(overflow_mode == QUEUE_DROP_OLDEST || overflow_mode == QUEUE_DROP_NEWEST,
              static_cast<FwAssertArgType>(overflow_mode));
    m_internal.setup(storage, total_needed_size);
    m_message_size = message_size;
    m_mode = mode;
    m_overflow_mode = overflow_mode;
}

Fw::SerializeStatus Queue::enqueue(const U8* const message, const FwSizeType size) {
    FW_ASSERT(m_message_size > 0, static_cast<FwAssertArgType>(m_message_size));  // Ensure initialization
    FW_ASSERT(m_message_size == size, static_cast<FwAssertArgType>(size),
              static_cast<FwAssertArgType>(m_message_size));  // Message size is as expected
    FW_ASSERT(message != nullptr);
    Fw::SerializeStatus status = m_internal.serialize(message, m_message_size);

    // If queue is full and we're in DROP_OLDEST mode, remove the oldest and try again
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && m_overflow_mode == QUEUE_DROP_OLDEST) {
        // Remove the oldest message by rotating
        Fw::SerializeStatus rotate_status = m_internal.rotate(m_message_size);
        if (rotate_status != Fw::FW_SERIALIZE_OK) {
            return rotate_status;
        }

        // Now enqueue the new message (should succeed since we just freed space)
        status = m_internal.serialize(message, m_message_size);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        } else {
            // Let the caller know we deleted data
            return Fw::FW_SERIALIZE_DISCARDED_EXISTING;
        }
    }

    return status;
}

Fw::SerializeStatus Queue::dequeue(U8* const message, const FwSizeType size) {
    FW_ASSERT(m_message_size > 0);  // Ensure initialization
    FW_ASSERT(m_message_size <= size, static_cast<FwAssertArgType>(size),
              static_cast<FwAssertArgType>(m_message_size));  // Sufficient storage space for read message
    FW_ASSERT(message != nullptr);
    Fw::SerializeStatus result;

    if (m_mode == QUEUE_FIFO) {
        // FIFO: Dequeue from the front (oldest message)
        result = m_internal.peek(message, m_message_size, 0);
        if (result != Fw::FW_SERIALIZE_OK) {
            return result;
        }
        return m_internal.rotate(m_message_size);
    } else {
        // LIFO: Dequeue from the back (newest message)
        FwSizeType current_size = m_internal.get_allocated_size();
        if (current_size < m_message_size) {
            return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
        }
        FwSizeType offset = current_size - m_message_size;
        result = m_internal.peek(message, m_message_size, offset);
        if (result != Fw::FW_SERIALIZE_OK) {
            return result;
        }
        return m_internal.trim(m_message_size);
    }
}

FwSizeType Queue::get_high_water_mark() const {
    FW_ASSERT(m_message_size > 0, static_cast<FwAssertArgType>(m_message_size));
    return m_internal.get_high_water_mark() / m_message_size;
}

void Queue::clear_high_water_mark() {
    m_internal.clear_high_water_mark();
}

FwSizeType Queue::getQueueSize() const {
    FW_ASSERT(m_message_size > 0, static_cast<FwAssertArgType>(m_message_size));
    return m_internal.get_allocated_size() / m_message_size;
}

}  // namespace Types
