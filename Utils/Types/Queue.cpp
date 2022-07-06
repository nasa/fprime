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

namespace Types {

Queue::Queue(U8* const storage, const FwSizeType depth, const FwSizeType message_size)
    : m_internal(storage, depth * message_size), m_message_size(message_size) {}

Fw::SerializeStatus Queue::enqueue(const U8* const message) {
    return m_internal.serialize(message, static_cast<NATIVE_UINT_TYPE>(m_message_size));
}

Fw::SerializeStatus Queue::dequeue(U8* const message) {
    Fw::SerializeStatus result = m_internal.peek(message, static_cast<NATIVE_UINT_TYPE>(m_message_size), 0);
    if (result != Fw::FW_SERIALIZE_OK) {
        return result;
    }
    return m_internal.rotate(m_message_size);
}
};  // namespace Types