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
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Types/CircularBuffer.hpp>

namespace Types {

CircularBuffer ::CircularBuffer()
    : m_store(nullptr), m_store_size(0), m_head_idx(0), m_allocated_size(0), m_high_water_mark(0) {}

CircularBuffer ::CircularBuffer(U8* const buffer, const FwSizeType size)
    : m_store(nullptr), m_store_size(0), m_head_idx(0), m_allocated_size(0), m_high_water_mark(0) {
    setup(buffer, size);
}

void CircularBuffer ::setup(U8* const buffer, const FwSizeType size) {
    FW_ASSERT(size > 0);
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(m_store == nullptr && m_store_size == 0);  // Not already setup

    // Initialize buffer data
    m_store = buffer;
    m_store_size = size;
    m_head_idx = 0;
    m_allocated_size = 0;
    m_high_water_mark = 0;
}

FwSizeType CircularBuffer ::get_allocated_size() const {
    return m_allocated_size;
}

FwSizeType CircularBuffer ::get_free_size() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    FW_ASSERT(m_allocated_size <= m_store_size, static_cast<FwAssertArgType>(m_allocated_size));
    return m_store_size - m_allocated_size;
}

FwSizeType CircularBuffer ::advance_idx(FwSizeType idx, FwSizeType amount) const {
    FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
    return (idx + amount) % m_store_size;
}

Fw::SerializeStatus CircularBuffer ::serialize(const U8* const buffer, const FwSizeType size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    FW_ASSERT(buffer != nullptr);
    // Check there is sufficient space
    if (size > get_free_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // Copy in all the supplied data
    FwSizeType idx = advance_idx(m_head_idx, m_allocated_size);
    for (FwSizeType i = 0; i < size; i++) {
        FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
        m_store[idx] = buffer[i];
        idx = advance_idx(idx);
    }
    m_allocated_size += size;
    FW_ASSERT(m_allocated_size <= this->get_capacity(), static_cast<FwAssertArgType>(m_allocated_size));
    m_high_water_mark = (m_high_water_mark > m_allocated_size) ? m_high_water_mark : m_allocated_size;
    return Fw::FW_SERIALIZE_OK;
}

template <typename T>
Fw::SerializeStatus CircularBuffer ::serialize_impl(const T& serializable, const FwSizeType size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient space
    if (size > get_free_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    const FwSizeType idx = advance_idx(m_head_idx, m_allocated_size);
    // Wrapping slot: serialize into a stack staging buffer, then byte-copy in with wrap-around
    if ((idx + size) > m_store_size) {
        FW_ASSERT(size <= STAGING_BUFFER_SIZE, static_cast<FwAssertArgType>(size),
                  static_cast<FwAssertArgType>(STAGING_BUFFER_SIZE));
        U8 staging[STAGING_BUFFER_SIZE > 0 ? STAGING_BUFFER_SIZE : 1] = {};
        Fw::ExternalSerializeBuffer stagingBuffer(staging, size);
        const Fw::SerializeStatus stagingStatus = stagingBuffer.serializeFrom(serializable);
        if (stagingStatus != Fw::FW_SERIALIZE_OK) {
            return stagingStatus;
        }
        return this->serialize(staging, size);
    }
    // Linear slot: serialize directly into the store
    Fw::ExternalSerializeBuffer slot(&m_store[idx], size);
    const Fw::SerializeStatus status = slot.serializeFrom(serializable);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    m_allocated_size += size;
    FW_ASSERT(m_allocated_size <= this->get_capacity(), static_cast<FwAssertArgType>(m_allocated_size));
    m_high_water_mark = (m_high_water_mark > m_allocated_size) ? m_high_water_mark : m_allocated_size;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer ::serialize(const Fw::Serializable& serializable, const FwSizeType size) {
    return this->serialize_impl(serializable, size);
}

Fw::SerializeStatus CircularBuffer ::serialize(const Fw::LinearBufferBase& buffer, const FwSizeType size) {
    return this->serialize_impl(buffer, size);
}

Fw::SerializeStatus CircularBuffer ::peek(char& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    return peek(reinterpret_cast<U8&>(value), offset);
}

Fw::SerializeStatus CircularBuffer ::peek(U8& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient data
    if ((sizeof(U8) + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    const FwSizeType idx = advance_idx(m_head_idx, offset);
    FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
    value = m_store[idx];
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer ::peek(U32& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient data
    if ((sizeof(U32) + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    value = 0;
    FwSizeType idx = advance_idx(m_head_idx, offset);

    // Deserialize all the bytes from network format
    for (FwSizeType i = 0; i < sizeof(U32); i++) {
        FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
        value = (value << 8) | static_cast<U32>(m_store[idx]);
        idx = advance_idx(idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer ::peek(U8* buffer, FwSizeType size, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    FW_ASSERT(buffer != nullptr);
    // Check there is sufficient data
    if ((size + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    FwSizeType idx = advance_idx(m_head_idx, offset);
    // Deserialize all the bytes from network format
    for (FwSizeType i = 0; i < size; i++) {
        FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
        buffer[i] = m_store[idx];
        idx = advance_idx(idx);
    }
    return Fw::FW_SERIALIZE_OK;
}

template <typename T>
Fw::SerializeStatus CircularBuffer ::peek_impl(T& serializable, FwSizeType size, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient data
    if ((size + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    const FwSizeType idx = advance_idx(m_head_idx, offset);
    // Wrapping slot: byte-copy out with wrap-around into a stack staging buffer, then deserialize
    if ((idx + size) > m_store_size) {
        FW_ASSERT(size <= STAGING_BUFFER_SIZE, static_cast<FwAssertArgType>(size),
                  static_cast<FwAssertArgType>(STAGING_BUFFER_SIZE));
        U8 staging[STAGING_BUFFER_SIZE > 0 ? STAGING_BUFFER_SIZE : 1];
        const Fw::SerializeStatus peekStatus = this->peek(staging, size, offset);
        if (peekStatus != Fw::FW_SERIALIZE_OK) {
            return peekStatus;
        }
        Fw::ExternalSerializeBuffer stagingBuffer(staging, size);
        const Fw::SerializeStatus status = stagingBuffer.setBuffLen(size);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        return stagingBuffer.deserializeTo(serializable);
    }
    // Linear slot: deserialize directly from the store
    Fw::ExternalSerializeBuffer slot(&m_store[idx], size);
    Fw::SerializeStatus status = slot.setBuffLen(size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    return slot.deserializeTo(serializable);
}

Fw::SerializeStatus CircularBuffer ::peek(Fw::Serializable& serializable, FwSizeType size, FwSizeType offset) const {
    return this->peek_impl(serializable, size, offset);
}

Fw::SerializeStatus CircularBuffer ::peek(Fw::LinearBufferBase& buffer, FwSizeType size, FwSizeType offset) const {
    return this->peek_impl(buffer, size, offset);
}

Fw::SerializeStatus CircularBuffer ::rotate(FwSizeType amount) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient data
    if (amount > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    m_head_idx = advance_idx(m_head_idx, amount);
    m_allocated_size -= amount;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer ::trim(FwSizeType amount) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    // Check there is sufficient data
    if (amount > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    // Simply reduce the allocated size without moving the head
    m_allocated_size -= amount;
    return Fw::FW_SERIALIZE_OK;
}

FwSizeType CircularBuffer ::get_capacity() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0);  // setup method was called
    return m_store_size;
}

FwSizeType CircularBuffer ::get_high_water_mark() const {
    return m_high_water_mark;
}

void CircularBuffer ::clear_high_water_mark() {
    m_high_water_mark = 0;
}

}  // End Namespace Types
