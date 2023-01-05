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

Queue::Queue() : m_internal(), m_message_size(0) {}

void Queue::setup(U8* const storage, const FwSizeType storage_size, const FwSizeType depth, const FwSizeType message_size) {
    // Ensure that enough storage was supplied
    const FwSizeType total_needed_size = depth * message_size;
    FW_ASSERT(storage_size >= total_needed_size, storage_size, depth, message_size);
    m_internal.setup(storage, total_needed_size);
    m_message_size = message_size;
}

Fw::SerializeStatus Queue::enqueue(const U8* const message, const FwSizeType size) {
    FW_ASSERT(m_message_size > 0, m_message_size); // Ensure initialization
    FW_ASSERT(m_message_size == size, size, m_message_size); // Message size is as expected
    return m_internal.serialize(message, static_cast<NATIVE_UINT_TYPE>(m_message_size));
}

Fw::SerializeStatus Queue::dequeue(U8* const message, const FwSizeType size) {
    FW_ASSERT(m_message_size > 0); // Ensure initialization
    FW_ASSERT(m_message_size <= size, size, m_message_size); // Sufficient storage space for read message
    Fw::SerializeStatus result = m_internal.peek(message, static_cast<NATIVE_UINT_TYPE>(m_message_size), 0);
    if (result != Fw::FW_SERIALIZE_OK) {
        return result;
    }
    return m_internal.rotate(m_message_size);
}

NATIVE_UINT_TYPE Queue::get_high_water_mark() const {
    FW_ASSERT(m_message_size > 0, m_message_size);
    return m_internal.get_high_water_mark() / m_message_size;
}

void Queue::clear_high_water_mark() {
    m_internal.clear_high_water_mark();
}

NATIVE_UINT_TYPE Queue::getQueueSize() const {
    FW_ASSERT(m_message_size > 0, m_message_size);
    return m_internal.get_allocated_size()/m_message_size;
}


};  // namespace Types