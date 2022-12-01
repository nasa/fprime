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
 *  Revised March 2022
 *      Author: bocchino
 */
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Types/CircularBuffer.hpp>

#ifdef CIRCULAR_DEBUG
    #include <Os/Log.hpp>
#endif

namespace Types {

CircularBuffer :: CircularBuffer() :
    m_store(nullptr),
    m_store_size(0),
    m_head_idx(0),
    m_allocated_size(0),
    m_high_water_mark(0)
{

}

CircularBuffer :: CircularBuffer(U8* const buffer, const NATIVE_UINT_TYPE size) :
    m_store(nullptr),
    m_store_size(0),
    m_head_idx(0),
    m_allocated_size(0),
    m_high_water_mark(0)
{
    setup(buffer, size);
}

void CircularBuffer :: setup(U8* const buffer, const NATIVE_UINT_TYPE size) {
    FW_ASSERT(size > 0);
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(m_store == nullptr && m_store_size == 0); // Not already setup

    // Initialize buffer data
    m_store = buffer;
    m_store_size = size;
    m_head_idx = 0;
    m_allocated_size = 0;
    m_high_water_mark = 0;
}

NATIVE_UINT_TYPE CircularBuffer :: get_allocated_size() const {
    return m_allocated_size;
}

NATIVE_UINT_TYPE CircularBuffer :: get_free_size() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_allocated_size <= m_store_size, m_allocated_size);
    return m_store_size - m_allocated_size;
}

NATIVE_UINT_TYPE CircularBuffer :: advance_idx(NATIVE_UINT_TYPE idx, NATIVE_UINT_TYPE amount) const {
    FW_ASSERT(idx < m_store_size, idx);
    return (idx + amount) % m_store_size;
}

Fw::SerializeStatus CircularBuffer :: serialize(const U8* const buffer, const NATIVE_UINT_TYPE size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(buffer != nullptr);
    // Check there is sufficient space
    if (size > get_free_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // Copy in all the supplied data
    NATIVE_UINT_TYPE idx = advance_idx(m_head_idx, m_allocated_size);
    for (U32 i = 0; i < size; i++) {
        FW_ASSERT(idx < m_store_size, idx);
        m_store[idx] = buffer[i];
        idx = advance_idx(idx);
    }
    m_allocated_size += size;
    FW_ASSERT(m_allocated_size <= this->get_capacity(), m_allocated_size);
    m_high_water_mark = (m_high_water_mark > m_allocated_size) ? m_high_water_mark : m_allocated_size;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(char& value, NATIVE_UINT_TYPE offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return peek(reinterpret_cast<U8&>(value), offset);
}

Fw::SerializeStatus CircularBuffer :: peek(U8& value, NATIVE_UINT_TYPE offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    // Check there is sufficient data
    if ((sizeof(U8) + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    const NATIVE_UINT_TYPE idx = advance_idx(m_head_idx, offset);
    FW_ASSERT(idx < m_store_size, idx);
    value = m_store[idx];
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U32& value, NATIVE_UINT_TYPE offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    // Check there is sufficient data
    if ((sizeof(U32) + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    value = 0;
    NATIVE_UINT_TYPE idx = advance_idx(m_head_idx, offset);

    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < sizeof(U32); i++) {
        FW_ASSERT(idx < m_store_size, idx);
        value = (value << 8) | static_cast<U32>(m_store[idx]);
        idx = advance_idx(idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U8* buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(buffer != nullptr);
    // Check there is sufficient data
    if ((size + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    NATIVE_UINT_TYPE idx = advance_idx(m_head_idx, offset);
    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < size; i++) {
        FW_ASSERT(idx < m_store_size, idx);
        buffer[i] = m_store[idx];
        idx = advance_idx(idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: rotate(NATIVE_UINT_TYPE amount) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    // Check there is sufficient data
    if (amount > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    m_head_idx = advance_idx(m_head_idx, amount);
    m_allocated_size -= amount;
    return Fw::FW_SERIALIZE_OK;
}

NATIVE_UINT_TYPE CircularBuffer ::get_capacity() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return m_store_size;
}

NATIVE_UINT_TYPE CircularBuffer ::get_high_water_mark() const {
    return m_high_water_mark;
}

void CircularBuffer ::clear_high_water_mark() {
    m_high_water_mark = 0;
}

#ifdef CIRCULAR_DEBUG
void CircularBuffer :: print() {
    NATIVE_UINT_TYPE idx = m_head_idx;
    Os::Log::logMsg("Ring: ", 0, 0, 0, 0, 0, 0);
    for (NATIVE_UINT_TYPE i = 0; i < m_allocated_size; ++i) {
        Os::Log::logMsg("%c", m_store[idx], 0, 0, 0, 0, 0);
        idx = advance_idx(idx);
    }
    Os::Log::logMsg("\n", 0, 0, 0, 0, 0, 0);
}
#endif
} //End Namespace Types
