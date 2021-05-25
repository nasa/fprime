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
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>

#if FW_SERIALIZABLE_TO_STRING
    #include <Fw/Types/EightyCharString.hpp>
#endif
#include <cstring>

namespace Fw {

Buffer::Buffer(): Serializable(),
    m_serialize_repr(),
    m_data(NULL),
    m_size(0),
    m_context(0xFFFFFFFF)
{}

Buffer::Buffer(const Buffer& src) : Serializable(),
    m_serialize_repr(src.m_data, src.m_size),
    m_data(src.m_data),
    m_size(src.m_size),
    m_context(src.m_context)
{}

Buffer::Buffer(U8* data, U32 size, U32 context) : Serializable(),
    m_serialize_repr(data, size),
    m_data(data),
    m_size(size),
    m_context(context)
{}

Buffer& Buffer::operator=(const Buffer& src) {
    // Ward against self-assignment
    if (this != &src) {
        this->set(src.m_data, src.m_size, src.m_context);
    }
    return *this;
}

bool Buffer::operator==(const Buffer& src) const {
    return (this->m_data == src.m_data) && (this->m_size == src.m_size) && (this->m_context == src.m_context);
}

U8* Buffer::getData() const {
    return this->m_data;
}

U32 Buffer::getSize() const {
    return this->m_size;
}

U32 Buffer::getContext() const {
    return this->m_context;
}

void Buffer::setData(U8* const data) {
    this->m_data = data;
    if (m_data != NULL) {
        this->m_serialize_repr.setExtBuffer(m_data, m_size);
    }
}

void Buffer::setSize(const U32 size) {
    this->m_size = size;
    if (m_data != NULL) {
        this->m_serialize_repr.setExtBuffer(m_data, m_size);
    }
}

void Buffer::setContext(const U32 context) {
    this->m_context = context;
}

void Buffer::set(U8* const data, const U32 size, const U32 context) {
    this->m_data = data;
    this->m_size = size;
    if (m_data != NULL) {
        this->m_serialize_repr.setExtBuffer(m_data, m_size);
    }
    this->m_context = context;
}

Fw::SerializeBufferBase& Buffer::getSerializeRepr() {
    return m_serialize_repr;
}

Fw::SerializeStatus Buffer::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat;
#if FW_SERIALIZATION_TYPE_ID
    stat = buffer.serialize((U32)Buffer::TYPE_ID);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
#endif
    stat = buffer.serialize(reinterpret_cast<POINTER_CAST>(this->m_data));
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_size);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_context);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

Fw::SerializeStatus Buffer::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    U32 typeId;

    stat = buffer.deserialize(typeId);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (typeId != Buffer::TYPE_ID) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
#endif
    POINTER_CAST pointer;
    stat = buffer.deserialize(pointer);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    this->m_data = reinterpret_cast<U8*>(pointer);

    stat = buffer.deserialize(this->m_size);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_context);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING  || BUILD_UT
void Buffer::toString(Fw::StringBase& text) const {
    static const char * formatString = "(data = %p, size = %u,context = %u)";
    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];

    (void)snprintf(outputString, FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE, formatString, this->m_data, this->m_size,
                   this->m_context);
    // Force NULL termination
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0;
    text = outputString;
}
#endif

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const Buffer& obj) {
        Fw::EightyCharString str;
        obj.toString(str);
        os << str.toChar();
        return os;
    }
#endif
} // end namespace Fw
