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
#include <cstring>
#include <Utils/Types/CircularBuffer.hpp>

namespace Types {

CircularBuffer :: CircularBuffer() :
    m_store(nullptr),
    m_store_size(0),
    m_head_idx(0),
    m_allocated_size(0),
    m_high_water_mark(0),
    m_ser_offset(0),
    m_deser_offset(0)
{

}

CircularBuffer :: CircularBuffer(U8* const buffer, const FwSizeType size) :
    m_store(nullptr),
    m_store_size(0),
    m_head_idx(0),
    m_allocated_size(0),
    m_high_water_mark(0),
    m_ser_offset(0),
    m_deser_offset(0)
{
    setup(buffer, size);
}

void CircularBuffer :: setup(U8* const buffer, const FwSizeType size) {
    FW_ASSERT(size > 0);
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(m_store == nullptr && m_store_size == 0); // Not already setup
    FW_ASSERT(size < static_cast<FwSizeType>(static_cast<FwSizeType>(-1) / 2)); // Reasonable size limit

    // Initialize buffer data
    m_store = buffer;
    m_store_size = size;
    m_head_idx = 0;
    m_allocated_size = 0;
    m_high_water_mark = 0;
    m_ser_offset = 0;
    m_deser_offset = 0;
}

FwSizeType CircularBuffer :: get_allocated_size() const {
    return m_allocated_size;
}

FwSizeType CircularBuffer :: get_free_size() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_allocated_size <= m_store_size, static_cast<FwAssertArgType>(m_allocated_size));
    return m_store_size - m_allocated_size;
}

FwSizeType CircularBuffer :: advance_idx(FwSizeType idx, FwSizeType amount) const {
    FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
    return (idx + amount) % m_store_size;
}

Fw::SerializeStatus CircularBuffer :: serialize(const U8* buffer, FwSizeType size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(buffer != nullptr || size == 0); // buffer must be valid unless size is 0
    FW_ASSERT(size <= m_store_size); // size cannot exceed total buffer capacity
    // Check there is sufficient space
    if (size > get_free_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // Copy in all the supplied data
    FwSizeType idx = advance_idx(m_head_idx, m_allocated_size);
    for (U32 i = 0; i < size; i++) {
        FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
        m_store[idx] = buffer[i];
        idx = advance_idx(idx);
    }
    m_allocated_size += size;
    FW_ASSERT(m_allocated_size <= this->get_capacity(), static_cast<FwAssertArgType>(m_allocated_size));
    m_high_water_mark = (m_high_water_mark > m_allocated_size) ? m_high_water_mark : m_allocated_size;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(char& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return peek(reinterpret_cast<U8&>(value), offset);
}

Fw::SerializeStatus CircularBuffer :: peek(U8& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    // Check there is sufficient data
    if ((sizeof(U8) + offset) > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    const FwSizeType idx = advance_idx(m_head_idx, offset);
    FW_ASSERT(idx < m_store_size, static_cast<FwAssertArgType>(idx));
    value = m_store[idx];
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer :: peek(U32& value, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
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

Fw::SerializeStatus CircularBuffer :: peek(U8* buffer, FwSizeType size, FwSizeType offset) const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(buffer != nullptr || size == 0); // buffer must be valid unless size is 0
    FW_ASSERT(size <= m_store_size); // size cannot exceed total buffer capacity
    FW_ASSERT(offset <= m_store_size); // offset cannot exceed total buffer capacity (allow testing edge cases)
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

Fw::SerializeStatus CircularBuffer :: rotate(FwSizeType amount) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(amount <= m_store_size); // amount cannot exceed total buffer capacity (allow testing edge cases)
    // Check there is sufficient data
    if (amount > m_allocated_size) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    m_head_idx = advance_idx(m_head_idx, amount);
    m_allocated_size -= amount;
    
    // Adjust offsets for the discarded data
    if (m_ser_offset >= amount) {
        m_ser_offset -= amount;  // Adjust for discarded data at front
    } else {
        m_ser_offset = 0;        // Was pointing to discarded data, reset to start
    }
    
    if (m_deser_offset >= amount) {
        m_deser_offset -= amount;  // Adjust for discarded data at front
    } else {
        m_deser_offset = 0;        // Was pointing to discarded data, reset to start
    }
    return Fw::FW_SERIALIZE_OK;
}

FwSizeType CircularBuffer ::get_capacity() const {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return m_store_size;
}

FwSizeType CircularBuffer ::get_high_water_mark() const {
    return m_high_water_mark;
}

void CircularBuffer ::clear_high_water_mark() {
    m_high_water_mark = 0;
}

// ----------------------------------------------------------------------
// SerializeBufferInterface implementation
// ----------------------------------------------------------------------

FwSizeType CircularBuffer::getBuffCapacity() const {
    return get_capacity();
}

FwSizeType CircularBuffer::getBuffLength() const {
    return get_allocated_size();
}

FwSizeType CircularBuffer::getBuffLeft() const {
    return get_allocated_size() - m_deser_offset;
}

U8* CircularBuffer::getBuffAddr() {
    // Return pointer to the underlying storage buffer
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return m_store;
}

const U8* CircularBuffer::getBuffAddr() const {
    // Return pointer to the underlying storage buffer
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    return m_store;
}

const U8* CircularBuffer::getBuffAddrLeft() const {
    // Return pointer to the remaining data (from deserialization position)
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_deser_offset <= get_allocated_size()); // offset should be valid

    return &m_store[advance_idx(m_head_idx, m_deser_offset)];
}

U8* CircularBuffer::getBuffAddrSer() {
    // Return pointer to the end of serialized data
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_ser_offset <= get_allocated_size()); // serialization offset should be valid

    return &m_store[advance_idx(m_head_idx, m_ser_offset)];
}

// Serialization methods
Fw::SerializeStatus CircularBuffer::serialize(U8 val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough space
    if (get_free_size() < sizeof(U8)) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Serialize the value using the existing serialize method
    Fw::SerializeStatus status = serialize(&val, sizeof(U8));
    if (status == Fw::FW_SERIALIZE_OK) {
        m_ser_offset = get_allocated_size();
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::serialize(I8 val) {
    return serialize(static_cast<U8>(val));
}

#if FW_HAS_16_BIT == 1
Fw::SerializeStatus CircularBuffer::serialize(U16 val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough space
    if (get_free_size() < sizeof(U16)) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Convert to network byte order (big endian)
    U8 bytes[2];
    bytes[0] = static_cast<U8>(val >> 8);
    bytes[1] = static_cast<U8>(val);
    
    // Serialize the bytes using the existing serialize method
    Fw::SerializeStatus status = serialize(bytes, sizeof(U16));
    if (status == Fw::FW_SERIALIZE_OK) {
        m_ser_offset = get_allocated_size();
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::serialize(I16 val) {
    return serialize(static_cast<U16>(val));
}
#endif

#if FW_HAS_32_BIT == 1
Fw::SerializeStatus CircularBuffer::serialize(U32 val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough space
    if (get_free_size() < sizeof(U32)) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Convert to network byte order (big endian)
    U8 bytes[4];
    bytes[0] = static_cast<U8>(val >> 24);
    bytes[1] = static_cast<U8>(val >> 16);
    bytes[2] = static_cast<U8>(val >> 8);
    bytes[3] = static_cast<U8>(val);
    
    // Serialize the bytes using the existing serialize method
    Fw::SerializeStatus status = serialize(bytes, sizeof(U32));
    if (status == Fw::FW_SERIALIZE_OK) {
        m_ser_offset = get_allocated_size();
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::serialize(I32 val) {
    return serialize(static_cast<U32>(val));
}
#endif

#if FW_HAS_64_BIT == 1
Fw::SerializeStatus CircularBuffer::serialize(U64 val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough space
    if (get_free_size() < sizeof(U64)) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Convert to network byte order (big endian)
    U8 bytes[8];
    bytes[0] = static_cast<U8>(val >> 56);
    bytes[1] = static_cast<U8>(val >> 48);
    bytes[2] = static_cast<U8>(val >> 40);
    bytes[3] = static_cast<U8>(val >> 32);
    bytes[4] = static_cast<U8>(val >> 24);
    bytes[5] = static_cast<U8>(val >> 16);
    bytes[6] = static_cast<U8>(val >> 8);
    bytes[7] = static_cast<U8>(val);
    
    // Serialize the bytes using the existing serialize method
    Fw::SerializeStatus status = serialize(bytes, sizeof(U64));
    if (status == Fw::FW_SERIALIZE_OK) {
        m_ser_offset = get_allocated_size();
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::serialize(I64 val) {
    return serialize(static_cast<U64>(val));
}
#endif

Fw::SerializeStatus CircularBuffer::serialize(F32 val) {
    // Convert float to U32 and serialize
    U32 u32Val;
    (void)memcpy(&u32Val, &val, sizeof(val));
    return serialize(u32Val);
}

Fw::SerializeStatus CircularBuffer::serialize(F64 val) {
    // Convert double to U64 and serialize
    U64 u64Val;
    (void)memcpy(&u64Val, &val, sizeof(val));
    return serialize(u64Val);
}

Fw::SerializeStatus CircularBuffer::serialize(bool val) {
    U8 boolVal = val ? 1 : 0;
    return serialize(boolVal);
}

Fw::SerializeStatus CircularBuffer::serialize(const void* val) {
    // Serialize pointer value (not contents)
    PlatformPointerCastType ptrVal = reinterpret_cast<PlatformPointerCastType>(val);
    return serialize(ptrVal);
}

Fw::SerializeStatus CircularBuffer::serialize(const U8* buff, FwSizeType length, Fw::Serialization::t mode) {
    FW_ASSERT(buff != nullptr || length == 0); // buffer must be valid unless length is 0
    
    // First serialize length if needed
    if (mode == Fw::Serialization::INCLUDE_LENGTH) {
        Fw::SerializeStatus status = serialize(static_cast<FwSizeStoreType>(length));
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }
    
    // Use the existing serialize method for the buffer
    return serialize(buff, length);
}

Fw::SerializeStatus CircularBuffer::serialize(const Fw::Serializable& val) {
    return val.serialize(*this);
}



Fw::SerializeStatus CircularBuffer::serializeSize(const FwSizeType size) {
    if ((size < std::numeric_limits<FwSizeStoreType>::min()) || 
        (size > std::numeric_limits<FwSizeStoreType>::max())) {
        return Fw::FW_SERIALIZE_FORMAT_ERROR;
    }
    return serialize(static_cast<FwSizeStoreType>(size));
}

// Deserialization methods
Fw::SerializeStatus CircularBuffer::deserialize(U8& val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (m_deser_offset >= get_allocated_size()) {
        return Fw::FW_DESERIALIZE_BUFFER_EMPTY;
    }
    
    Fw::SerializeStatus status = peek(val, m_deser_offset);
    if (status == Fw::FW_SERIALIZE_OK) {
        m_deser_offset++;
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(I8& val) {
    U8 u8Val;
    Fw::SerializeStatus status = deserialize(u8Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = static_cast<I8>(u8Val);
    }
    return status;
}

#if FW_HAS_16_BIT == 1
Fw::SerializeStatus CircularBuffer::deserialize(U16& val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (m_deser_offset + sizeof(U16) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    U8 bytes[2];
    Fw::SerializeStatus status = peek(bytes, sizeof(U16), m_deser_offset);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = static_cast<U16>((static_cast<U16>(bytes[0]) << 8) | static_cast<U16>(bytes[1]));
        m_deser_offset += sizeof(U16);
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(I16& val) {
    U16 u16Val;
    Fw::SerializeStatus status = deserialize(u16Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = static_cast<I16>(u16Val);
    }
    return status;
}
#endif

#if FW_HAS_32_BIT == 1
Fw::SerializeStatus CircularBuffer::deserialize(U32& val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (m_deser_offset + sizeof(U32) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    U8 bytes[4];
    Fw::SerializeStatus status = peek(bytes, sizeof(U32), m_deser_offset);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = (static_cast<U32>(bytes[0]) << 24) | 
              (static_cast<U32>(bytes[1]) << 16) | 
              (static_cast<U32>(bytes[2]) << 8) | 
              static_cast<U32>(bytes[3]);
        m_deser_offset += sizeof(U32);
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(I32& val) {
    U32 u32Val;
    Fw::SerializeStatus status = deserialize(u32Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = static_cast<I32>(u32Val);
    }
    return status;
}
#endif

#if FW_HAS_64_BIT == 1
Fw::SerializeStatus CircularBuffer::deserialize(U64& val) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (m_deser_offset + sizeof(U64) > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    U8 bytes[8];
    Fw::SerializeStatus status = peek(bytes, sizeof(U64), m_deser_offset);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = (static_cast<U64>(bytes[0]) << 56) | 
              (static_cast<U64>(bytes[1]) << 48) | 
              (static_cast<U64>(bytes[2]) << 40) | 
              (static_cast<U64>(bytes[3]) << 32) |
              (static_cast<U64>(bytes[4]) << 24) | 
              (static_cast<U64>(bytes[5]) << 16) | 
              (static_cast<U64>(bytes[6]) << 8) | 
              static_cast<U64>(bytes[7]);
        m_deser_offset += sizeof(U64);
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(I64& val) {
    U64 u64Val;
    Fw::SerializeStatus status = deserialize(u64Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = static_cast<I64>(u64Val);
    }
    return status;
}
#endif

Fw::SerializeStatus CircularBuffer::deserialize(F32& val) {
    U32 u32Val;
    Fw::SerializeStatus status = deserialize(u32Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        (void)memcpy(&val, &u32Val, sizeof(val));
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(F64& val) {
    U64 u64Val;
    Fw::SerializeStatus status = deserialize(u64Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        (void)memcpy(&val, &u64Val, sizeof(val));
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(bool& val) {
    U8 u8Val;
    Fw::SerializeStatus status = deserialize(u8Val);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = (u8Val != 0);
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(void*& val) {
    PlatformPointerCastType ptrVal;
    Fw::SerializeStatus status = deserialize(ptrVal);
    if (status == Fw::FW_SERIALIZE_OK) {
        val = reinterpret_cast<void*>(ptrVal);
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(U8* buff, FwSizeType& length) {
    return deserialize(buff, length, Fw::Serialization::INCLUDE_LENGTH);
}

Fw::SerializeStatus CircularBuffer::deserialize(U8* buff, FwSizeType& length, Fw::Serialization::t mode) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(buff != nullptr || length == 0); // buffer must be valid unless length is 0
    FW_ASSERT(length <= m_store_size); // length cannot exceed total buffer capacity
    
    if (mode == Fw::Serialization::INCLUDE_LENGTH) {
        FwSizeStoreType storedLength;
        Fw::SerializeStatus status = deserialize(storedLength);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
        
        if (storedLength > getBuffLeft() || storedLength > length) {
            return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
        }
        
        length = static_cast<FwSizeType>(storedLength);
    }
    
    if (length > getBuffLeft()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    Fw::SerializeStatus status = peek(buff, length, m_deser_offset);
    if (status == Fw::FW_SERIALIZE_OK) {
        m_deser_offset += length;
    }
    return status;
}

Fw::SerializeStatus CircularBuffer::deserialize(Fw::Serializable& val) {
    return val.deserialize(*this);
}



Fw::SerializeStatus CircularBuffer::deserializeSize(FwSizeType& size) {
    FwSizeStoreType storedSize = 0;
    Fw::SerializeStatus status = deserialize(storedSize);
    if (status == Fw::FW_SERIALIZE_OK) {
        size = static_cast<FwSizeType>(storedSize);
    }
    return status;
}

// Buffer management methods
void CircularBuffer::resetSer() {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    m_ser_offset = 0;
    m_deser_offset = 0;
}

void CircularBuffer::resetDeser() {
    m_deser_offset = 0;
}

Fw::SerializeStatus CircularBuffer::moveSerToOffset(FwSizeType offset) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (offset > get_allocated_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    m_ser_offset = offset;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::moveDeserToOffset(FwSizeType offset) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    if (offset > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    m_deser_offset = offset;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::serializeSkip(FwSizeType numBytesToSkip) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_ser_offset <= get_allocated_size()); // current offset should be valid
    FW_ASSERT(numBytesToSkip <= m_store_size); // skip amount cannot exceed total buffer capacity
    
    // Check for potential overflow in addition
    if ((numBytesToSkip > 0) && (m_ser_offset > (static_cast<FwSizeType>(-1) - numBytesToSkip))) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;  // Would overflow
    }
    
    FwSizeType newSerOffset = m_ser_offset + numBytesToSkip;
    if (newSerOffset > get_allocated_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    m_ser_offset = newSerOffset;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::deserializeSkip(FwSizeType numBytesToSkip) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(m_deser_offset <= get_allocated_size()); // current offset should be valid
    FW_ASSERT(numBytesToSkip <= m_store_size); // skip amount cannot exceed total buffer capacity
    
    // Check for potential overflow in addition
    if ((numBytesToSkip > 0) && (m_deser_offset > (static_cast<FwSizeType>(-1) - numBytesToSkip))) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;  // Would overflow
    }
    
    FwSizeType newDeserOffset = m_deser_offset + numBytesToSkip;
    if (newDeserOffset > get_allocated_size()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    m_deser_offset = newDeserOffset;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::setBuff(const U8* src, FwSizeType length) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(src != nullptr); // source buffer must be valid
    
    // Reset the buffer
    resetSer();
    resetDeser();
    
    // Check if the data fits
    if (length > get_capacity()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Serialize the data
    return serialize(src, length, Fw::Serialization::OMIT_LENGTH);
}

Fw::SerializeStatus CircularBuffer::setBuffLen(FwSizeType length) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    FW_ASSERT(length <= m_store_size); // length cannot exceed total buffer capacity
    
    if (length > get_allocated_size()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    m_ser_offset = length;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::copyRaw(Fw::SerializeBufferBase& dest, FwSizeType size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough data to copy
    if (size > getBuffLeft()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    // Check if destination has enough space
    if (size > dest.getBuffCapacity()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Extract data from circular buffer into a linear temporary buffer
    U8 temp_buffer[FW_COM_BUFFER_MAX_SIZE]; // Maximum F' communication buffer size
    FW_ASSERT(size <= sizeof(temp_buffer)); // Size limitation
    
    // Copy data from current deserialization position
    for (FwSizeType i = 0; i < size; i++) {
        FwSizeType idx = advance_idx(m_head_idx, m_deser_offset + i);
        temp_buffer[i] = m_store[idx];
    }
    
    // Reset destination and serialize the data to it 
    dest.resetSer();
    dest.resetDeser();
    Fw::SerializeStatus status = dest.serialize(temp_buffer, size, Fw::Serialization::OMIT_LENGTH);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    
    // Advance our deserialization pointer
    m_deser_offset += size;
    
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CircularBuffer::copyRawOffset(Fw::SerializeBufferBase& dest, FwSizeType size) {
    FW_ASSERT(m_store != nullptr && m_store_size != 0); // setup method was called
    
    // Check if there's enough data to copy
    if (size > getBuffLeft()) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    
    // Check if destination has enough space for appending
    if (size > dest.getBuffCapacity() - dest.getBuffLength()) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }
    
    // Extract data from circular buffer 
    U8 temp_buffer[FW_COM_BUFFER_MAX_SIZE]; // Maximum F' communication buffer size
    FW_ASSERT(size <= sizeof(temp_buffer)); // Size limitation
    
    // Copy data from current deserialization position
    for (FwSizeType i = 0; i < size; i++) {
        FwSizeType idx = advance_idx(m_head_idx, m_deser_offset + i);
        temp_buffer[i] = m_store[idx];
    }
    
    // Append to destination buffer using serialize 
    Fw::SerializeStatus status = dest.serialize(temp_buffer, size, Fw::Serialization::OMIT_LENGTH);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    
    // Note: Do NOT advance deserialization pointer for copyRawOffset
    
    return Fw::FW_SERIALIZE_OK;
}

// Override assignment operator to use base class copyFrom
Fw::SerializeBufferBase& CircularBuffer::operator=(const Fw::SerializeBufferBase& src) {
    // Use base class assignment which calls copyFrom internally
    return Fw::SerializeBufferBase::operator=(src);
}


} //End Namespace Types
