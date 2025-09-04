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

Buffer::Buffer() : Serializable(), m_serialize_repr(), m_bufferData(nullptr), m_size(0), m_context(0xFFFFFFFF) {}

Buffer::Buffer(const Buffer& src)
    : Serializable(), m_serialize_repr(), m_bufferData(src.m_bufferData), m_size(src.m_size), m_context(src.m_context) {
    if (src.m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(src.m_bufferData, src.m_size);
    }
}

Buffer::Buffer(U8* data, FwSizeType size, U32 context)
    : Serializable(), m_serialize_repr(), m_bufferData(data), m_size(size), m_context(context) {
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
}

Buffer& Buffer::operator=(const Buffer& src) {
    // Ward against self-assignment
    if (this != &src) {
        this->set(src.m_bufferData, src.m_size, src.m_context);
    }
    return *this;
}

bool Buffer::operator==(const Buffer& src) const {
    return (this->m_bufferData == src.m_bufferData) && (this->m_size == src.m_size) &&
           (this->m_context == src.m_context);
}

bool Buffer::isValid() const {
    return (this->m_bufferData != nullptr) && (this->m_size > 0);
}

U8* Buffer::getData() const {
    return this->m_bufferData;
}

FwSizeType Buffer::getSize() const {
    return this->m_size;
}

U32 Buffer::getContext() const {
    return this->m_context;
}

void Buffer::setData(U8* const data) {
    this->m_bufferData = data;
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
}

void Buffer::setSize(const FwSizeType size) {
    this->m_size = size;
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
}

void Buffer::setContext(const U32 context) {
    this->m_context = context;
}

void Buffer::set(U8* const data, const FwSizeType size, const U32 context) {
    this->m_bufferData = data;
    this->m_size = size;
    if (m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
    this->m_context = context;
}

Fw::ExternalSerializeBufferWithMemberCopy Buffer::getSerializer() {
    if (this->isValid()) {
        Fw::ExternalSerializeBufferWithMemberCopy esb(this->m_bufferData, this->m_size);
        esb.resetSer();
        return esb;
    } else {
        return ExternalSerializeBufferWithMemberCopy();
    }
}

Fw::ExternalSerializeBufferWithMemberCopy Buffer::getDeserializer() {
    if (this->isValid()) {
        Fw::ExternalSerializeBufferWithMemberCopy esb(this->m_bufferData, this->m_size);
        Fw::SerializeStatus stat = esb.setBuffLen(this->m_size);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK);
        return esb;
    } else {
        return ExternalSerializeBufferWithMemberCopy();
    }
}

Fw::SerializeStatus Buffer::serializeTo(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat;
#if FW_SERIALIZATION_TYPE_ID
    stat = buffer.serializeFrom(static_cast<U32>(Buffer::TYPE_ID));
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
#endif
    stat = buffer.serializeFrom(reinterpret_cast<PlatformPointerCastType>(this->m_bufferData));
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_size);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serializeFrom(this->m_context);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

Fw::SerializeStatus Buffer::deserializeFrom(Fw::SerializeBufferBase& buffer) {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    U32 typeId;

    stat = buffer.deserializeTo(typeId);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (typeId != Buffer::TYPE_ID) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
#endif
    PlatformPointerCastType pointer;
    stat = buffer.deserializeTo(pointer);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    this->m_bufferData = reinterpret_cast<U8*>(pointer);

    stat = buffer.deserializeTo(this->m_size);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserializeTo(this->m_context);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (this->m_bufferData != nullptr) {
        this->m_serialize_repr.setExtBuffer(this->m_bufferData, this->m_size);
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING
void Buffer::toString(Fw::StringBase& text) const {
    static const char* formatString = "(data = %p, size = %u, context = %u)";
    text.format(formatString, this->m_bufferData, this->m_size, this->m_context);
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
