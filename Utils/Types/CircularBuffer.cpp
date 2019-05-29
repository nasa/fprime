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

#include <stdio.h>


namespace Types {

CircularBuffer :: CircularBuffer(U8* const buffer, const NATIVE_UINT_TYPE size) :
    m_store(buffer),
    m_size(size),
    m_head(m_store),
    m_tail(m_store)
{}

NATIVE_UINT_TYPE CircularBuffer :: get_remaining_size(bool serialization) {
    // Note: a byte is lost in order to prevent wrap-around confusion
    const NATIVE_UINT_TYPE remaining = (m_tail >= m_head) ?
        (m_size - 1 - (reinterpret_cast<POINTER_CAST>(m_tail) - reinterpret_cast<POINTER_CAST>(m_head))) :
        (reinterpret_cast<POINTER_CAST>(m_head) - reinterpret_cast<POINTER_CAST>(m_tail) - 1);
    FW_ASSERT(remaining != static_cast<NATIVE_UINT_TYPE>(-1));
    return serialization ? remaining : m_size - 1 - remaining;
}

U8* CircularBuffer :: increment(U8* const pointer, NATIVE_UINT_TYPE amount) {
    U8* ret = pointer;
    //TODO: need O(1) implementation here
    for (NATIVE_UINT_TYPE i = 0; i < amount; i++) {
        ret = ret + 1;
        if (ret >= (m_store + m_size)) {
            ret = m_store;
        }
    }
    return ret;
}

Fw::SerializeStatus CircularBuffer :: serialize(const U8* const buffer, const NATIVE_UINT_TYPE size) {
    // Check that the head and tail pointers are consistent
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    // Check there is sufficient space
    if (size > get_remaining_size(true)) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // Copy in all the supplied data
    for (U32 i = 0; i < size; i++) {
        *m_tail = buffer[i];
        // Wrap-around increment of tail
        m_tail = increment(m_tail);
        FW_ASSERT(m_tail != m_head,
                reinterpret_cast<POINTER_CAST>(m_tail),
                reinterpret_cast<POINTER_CAST>(m_head));
    }
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U8& value, NATIVE_UINT_TYPE offset) {
    // Check that the head and tail pointers are consistent
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    // Check there is sufficient data
    if (sizeof(U8) > get_remaining_size(false)) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    value = *m_head;
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U32& value, NATIVE_UINT_TYPE offset) {
    // Check that the head and tail pointers are consistent
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    // Check there is sufficient data
    if (sizeof(U32) > get_remaining_size(false)) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    U8* peeker = m_head;
    value = 0;
    peeker = increment(peeker, offset);
    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < sizeof(U32); i++) {
        value = (value << 8) | static_cast<U32>(*peeker);
        peeker = increment(peeker);
    }
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U8* buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset) {
    // Check that the head and tail pointers are consistent
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    // Check there is sufficient data
    if (size > get_remaining_size(false)) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    U8* peeker = m_head;
    peeker = increment(peeker, offset);
    // Deserialize all the bytes from network format
    for (NATIVE_UINT_TYPE i = 0; i < size; i++) {
        *buffer = *peeker;
        peeker = increment(peeker);
        buffer = buffer + 1;
    }
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: rotate(NATIVE_UINT_TYPE amount) {
    // Check that the head and tail pointers are consistent
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    // Check there is sufficient data
    if (amount > get_remaining_size(false)) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    m_head = increment(m_head, amount);
    ASSERT_CONSISTENT(m_store, m_size, m_head);
    ASSERT_CONSISTENT(m_store, m_size, m_tail);
    return Fw::FW_SERIALIZE_OK;
}

#ifdef CIRCULAR_DEBUG
void CircularBuffer :: print() {
    U8* pointer = m_head;
    Os::Log::logMsg("Ring: ", 0, 0, 0, 0, 0, 0);
    while (pointer != m_tail) {
    	Os::Log::logMsg("%c", *pointer, 0, 0, 0, 0, 0);
        pointer = increment(pointer);
    }
    Os::Log::logMsg("\n", 0, 0, 0, 0, 0, 0);
}
#endif
} //End Namespace Types
