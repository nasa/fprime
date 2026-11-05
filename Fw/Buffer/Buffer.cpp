// ======================================================================
// \title  Buffer.cpp
// \author mstarch
// \brief  cpp file for Fw::Buffer implementation
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>

#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/String.hpp>
#endif
#include <cstring>

namespace Fw {

Buffer::Buffer()
    : Serializable(),
      m_serialize_repr(),
      m_bufferData(nullptr),
      m_offset(0),
      m_size(0),
      m_capacity(0),
      m_context(0xFFFFFFFF) {}

Buffer::Buffer(const Buffer& src)
    : Serializable(),
      m_serialize_repr(),
      m_bufferData(src.m_bufferData),
      m_offset(src.m_offset),
      m_size(src.m_size),
      m_capacity(src.m_capacity),
      m_context(src.m_context) {
    if (src.m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData + this->m_offset, this->m_size);
    }
}

Buffer::Buffer(U8* data, FwSizeType size, U32 context)
    : Serializable(),
      m_serialize_repr(),
      m_bufferData(data),
      m_offset(0),
      m_size(size),
      m_capacity(size),
      m_context(context) {
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
}

Buffer& Buffer::operator=(const Buffer& src) {
    // Ward against self-assignment
    if (this != &src) {
        this->m_bufferData = src.m_bufferData;
        this->m_offset = src.m_offset;
        this->m_size = src.m_size;
        this->m_capacity = src.m_capacity;
        this->m_context = src.m_context;
        if (this->m_bufferData != nullptr) {
            this->m_serialize_repr.setExtBuffer(this->m_bufferData + this->m_offset, this->m_size);
        }
    }
    return *this;
}

bool Buffer::operator==(const Buffer& src) const {
    return (this->m_bufferData == src.m_bufferData) && (this->m_offset == src.m_offset) &&
           (this->m_size == src.m_size) && (this->m_capacity == src.m_capacity) && (this->m_context == src.m_context);
}

bool Buffer::isValid() const {
    return (this->m_bufferData != nullptr) && (this->m_size > 0);
}

U8* Buffer::getData() const {
    return (this->m_bufferData == nullptr) ? nullptr : (this->m_bufferData + this->m_offset);
}

U8* Buffer::getOriginalData() const {
    return this->m_bufferData;
}

FwSizeType Buffer::getSize() const {
    return this->m_size;
}

FwSizeType Buffer::getCapacity() const {
    return this->m_capacity;
}

FwSizeType Buffer::getOffset() const {
    return this->m_offset;
}

U32 Buffer::getContext() const {
    return this->m_context;
}

void Buffer::advance(const FwSignedSizeType amount) {
    FW_ASSERT(this->m_bufferData != nullptr);
    // New offset must remain within [0, capacity]
    if (amount < 0) {
        FW_ASSERT(this->m_offset >= static_cast<FwSizeType>(-amount), static_cast<FwAssertArgType>(this->m_offset),
                  static_cast<FwAssertArgType>(amount));
    } else {
        // Advancing must not move past the end of the represented data
        FW_ASSERT(static_cast<FwSizeType>(amount) <= this->m_size, static_cast<FwAssertArgType>(this->m_size),
                  static_cast<FwAssertArgType>(amount));
    }
    this->m_offset = static_cast<FwSizeType>(static_cast<FwSignedSizeType>(this->m_offset) + amount);
    // Keep the end of the represented data fixed
    this->m_size = static_cast<FwSizeType>(static_cast<FwSignedSizeType>(this->m_size) - amount);
    FW_ASSERT(this->m_offset + this->m_size <= this->m_capacity, static_cast<FwAssertArgType>(this->m_offset),
              static_cast<FwAssertArgType>(this->m_size), static_cast<FwAssertArgType>(this->m_capacity));
    this->m_serialize_repr.setExtBuffer(this->m_bufferData + this->m_offset, this->m_size);
}

void Buffer::setData(U8* const data) {
    FW_ASSERT(this->m_bufferData != nullptr);
    FW_ASSERT(data >= this->m_bufferData && data <= &this->m_bufferData[this->m_capacity]);
    this->advance(static_cast<FwSignedSizeType>(data - (this->m_bufferData + this->m_offset)));
}

void Buffer::setSize(const FwSizeType size) {
    FW_ASSERT(this->m_offset + size <= this->m_capacity, static_cast<FwAssertArgType>(this->m_offset),
              static_cast<FwAssertArgType>(size), static_cast<FwAssertArgType>(this->m_capacity));
    this->m_size = size;
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData + this->m_offset, this->m_size);
    }
}

void Buffer::setContext(const U32 context) {
    this->m_context = context;
}

void Buffer::set(U8* const data, const FwSizeType size, const U32 context) {
    this->m_bufferData = data;
    this->m_offset = 0;
    this->m_size = size;
    this->m_capacity = size;
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
    this->m_context = context;
}

Fw::ExternalSerializeBufferWithMemberCopy Buffer::getSerializer() {
    if (this->isValid()) {
        Fw::ExternalSerializeBufferWithMemberCopy esb(this->m_bufferData + this->m_offset, this->m_size);
        esb.resetSer();
        return esb;
    } else {
        return ExternalSerializeBufferWithMemberCopy();
    }
}

Fw::ExternalSerializeBufferWithMemberCopy Buffer::getDeserializer() {
    if (this->isValid()) {
        Fw::ExternalSerializeBufferWithMemberCopy esb(this->m_bufferData + this->m_offset, this->m_size);
        Fw::SerializeStatus stat = esb.setBuffLen(this->m_size);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK);
        return esb;
    } else {
        return ExternalSerializeBufferWithMemberCopy();
    }
}

Fw::SerializeStatus Buffer::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    Fw::SerializeStatus stat;
    stat = buffer.serializeFrom(reinterpret_cast<PlatformPointerCastType>(this->m_bufferData), mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_size, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_context, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_offset, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_capacity, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

Fw::SerializeStatus Buffer::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    Fw::SerializeStatus stat;
    PlatformPointerCastType pointer;
    stat = buffer.deserializeTo(pointer, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    this->m_bufferData = reinterpret_cast<U8*>(pointer);

    stat = buffer.deserializeTo(this->m_size, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserializeTo(this->m_context, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserializeTo(this->m_offset, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserializeTo(this->m_capacity, mode);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (this->m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData + this->m_offset, this->m_size);
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING
void Buffer::toString(Fw::StringBase& text) const {
    static const char* formatString = "(data = %p, size = %u, context = %u, offset = %u, capacity = %u)";
    (void)text.format(formatString, this->m_bufferData, this->m_size, this->m_context, this->m_offset,
                      this->m_capacity);  // display string may safely truncate
}
#endif

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const Buffer& obj) {
    Fw::String str;
    obj.toString(str);
    os << str.toChar();
    return os;
}
#endif

}  // end namespace Fw
