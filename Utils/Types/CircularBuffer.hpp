/*
 * CircularBuffer.hpp:
 *
 * Buffer used to efficiently store data in ring data structure. Uses an externally supplied
 * data store as the backing for this buffer. Thus it is dependent on receiving sole ownership
 * of the supplied buffer.
 *
 * Note: this given implementation loses one byte of the data store in order to ensure that a
 * separate wrap-around tracking variable is not needed.
 *
 *  Created on: Apr 4, 2019
 *      Author: lestarch
 */
#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>

#ifndef TYPES_CIRCULAR_BUFFER_HPP
#define TYPES_CIRCULAR_BUFFER_HPP

#define CIRCULAR_DEBUG

// An assertion to guarantee the self-consistency of a head/tail pointer w.r.t. the store and size
#define ASSERT_CONSISTENT(store, size, X) \
    FW_ASSERT(X >= store && X < (store + size), \
              reinterpret_cast<POINTER_CAST>(X), \
              reinterpret_cast<POINTER_CAST>(store))

namespace Types {

class CircularBuffer {
    public:
        /**
         * Circular buffer constructor. Wraps the supplied buffer as the new data store. Buffer
         * size is supplied in the 'size' argument.
         *
         * Note: ownership of the supplied buffer is held until the circular buffer is destructed.
         *
         * \param const U8* buffer: supplied buffer used as a data store.
         * \param NATIVE_UINT_TYPE size: size of the supplied data store.
         */
        CircularBuffer(U8* const buffer, const NATIVE_UINT_TYPE size);

        /**
         * Serialize a given buffer into this this circular buffer. Will not accept more data then
         * space available. This means it will not overwrite existing data.
         * \param const U8* buffer: supplied buffer to be serialized.
         * \param NATIVE_UINT_TYPE size: size of the supplied buffer.
         */
        Fw::SerializeStatus serialize(const U8* const buffer, const NATIVE_UINT_TYPE size);

        /**
         * Deserialize data into the given variable without moving the head pointer
         * \param U8& value: value to fill
         */
        Fw::SerializeStatus peek(char& value, NATIVE_UINT_TYPE offset = 0);
        /**
         * Deserialize data into the given variable without moving the head pointer
         * \param U8& value: value to fill
         */
        Fw::SerializeStatus peek(U8& value, NATIVE_UINT_TYPE offset = 0);
        /**
         * Deserialize data into the given variable without moving the head pointer
         * \param U32& value: value to fill
         */
        Fw::SerializeStatus peek(U32& value, NATIVE_UINT_TYPE offset = 0);

        /**
         * Deserialize data into the given buffer without moving the head variable.
         * \param U32& value: value to fill
         */
        Fw::SerializeStatus peek(U8* buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset = 0);

        /**
         * Rotate the head pointer effectively erasing data from the circular buffer and making
         * space. Cannot rotate more than the available space.
         * \param NATIVE_UINT_TYPE amount: amount to rotate by (in bytes)
         */
        Fw::SerializeStatus rotate(NATIVE_UINT_TYPE amount);

        /**
         * Get the remaining size in this circular buffer.
         * \param bool serialization: is this serialization (opposed to deserialization)
         */
        NATIVE_UINT_TYPE get_remaining_size(bool serialization = false);

#ifdef CIRCULAR_DEBUG
        void print();
#endif
    private:
        /**
         * Returns a wrap-incremented pointer.
         * \param const U8* pointer: pointer to increment and wrap.
         * \param NATIVE_UINT_TYPE amount: amount to increment
         * \return: new pointer value
         */
        U8* increment(U8* const pointer, NATIVE_UINT_TYPE amount = 1);
        //! Memory store backing this circular buffer
        U8* const m_store;
        //! Size of the backed data store
        const NATIVE_UINT_TYPE m_size;
        //! Head pointer. As items are deserialized, this will move forward.
        U8* m_head;
        //! Tail pointer. As items are serialized, this will move forward.
        U8* m_tail;
};
} //End Namespace Types
#endif

