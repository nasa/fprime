/*
 * CircularBuffer.cpp:
 *
 * Buffer used to efficiently store data in ring data structure. Uses an externally supplied
 * data store as the backing for this buffer. Thus it is dependent on receiving sole ownership
 * of the supplied buffer.
 *
 * This implementation file contains the function definitions.
 *
 *  Created on: Apr 4, 2019
 *      Author: lestarch
 */
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Types/CircularBuffer.hpp>

#ifdef CIRCULAR_DEBUG
    #include <Os/Log.hpp>
#endif

#include <cstdio>

namespace Types {

CircularBuffer :: CircularBuffer(U8* const buffer, const NATIVE_UINT_TYPE size) :
    m_store(buffer),
    m_allocated_size(0),
    m_store_size(size),
    m_head_idx(0),
    m_tail_idx(0)
{
  FW_ASSERT(m_store_size > 0);
}

NATIVE_UINT_TYPE CircularBuffer :: get_allocated_size() const {
    return m_allocated_size;
}

NATIVE_UINT_TYPE CircularBuffer :: get_free_size() const {
    FW_ASSERT(m_allocated_size <= m_store_size);
    return this->get_capacity() - m_allocated_size;
}

NATIVE_UINT_TYPE CircularBuffer :: increment_idx(NATIVE_UINT_TYPE idx, NATIVE_UINT_TYPE amount) const {
    FW_ASSERT(idx < m_store_size);
    return (idx + amount) % m_store_size;
}

Fw::SerializeStatus CircularBuffer :: serialize(const U8* const buffer, const NATIVE_UINT_TYPE size) {
    // Check there is sufficient space
    if (size > get_free_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // Copy in all the supplied data
    for (U32 i = 0; i < size; i++) {
        m_store[m_tail_idx] = buffer[i];
        // Wrap-around increment of tail
        m_tail_idx = increment_idx(m_tail_idx);
    }
    m_allocated_size += size;
    FW_ASSERT(m_allocated_size <= this->get_capacity());
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(char& value, NATIVE_UINT_TYPE offset) const {
    return peek(reinterpret_cast<U8&>(value), offset);
}

Fw::SerializeStatus CircularBuffer :: peek(U8& value, NATIVE_UINT_TYPE offset) const {
    // Check there is sufficient data
    if ((sizeof(U8) + offset) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    const NATIVE_UINT_TYPE idx = increment_idx(m_head_idx, offset);
    FW_ASSERT(idx < m_store_size, idx);
    value = m_store[idx];
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U32& value, NATIVE_UINT_TYPE offset) const {
    // Check there is sufficient data
    if ((sizeof(U32) + offset) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    value = 0;
    NATIVE_UINT_TYPE peeker_idx = increment_idx(m_head_idx, offset);

    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < sizeof(U32); i++) {
        FW_ASSERT(peeker_idx < m_store_size);
        value = (value << 8) | static_cast<U32>(m_store[peeker_idx]);
        peeker_idx = increment_idx(peeker_idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U8* buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset) const {
    // Check there is sufficient data
    if ((size + offset) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    NATIVE_UINT_TYPE peeker_idx = increment_idx(m_head_idx, offset);
    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < size; i++) {
        FW_ASSERT(peeker_idx < m_store_size);
        buffer[i] = m_store[peeker_idx];
        peeker_idx = increment_idx(peeker_idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: rotate(NATIVE_UINT_TYPE amount) {
    // Check there is sufficient data
    if (amount > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    m_head_idx = increment_idx(m_head_idx, amount);
    m_allocated_size -= amount;
    return Fw::FW_SERIALIZE_OK;
}

NATIVE_UINT_TYPE CircularBuffer ::get_capacity() const {
    return m_store_size;
}

#ifdef CIRCULAR_DEBUG
void CircularBuffer :: print() {
    NATIVE_UINT_TYPE pointer_idx = m_head_idx;
    Os::Log::logMsg("Ring: ", 0, 0, 0, 0, 0, 0);
    while (pointer_idx != m_tail_idx) {
    	Os::Log::logMsg("%c", m_store[pointer_idx], 0, 0, 0, 0, 0);
        pointer_idx = increment_idx(pointer_idx);
    }
    Os::Log::logMsg("\n", 0, 0, 0, 0, 0, 0);
}
#endif
} //End Namespace Types
