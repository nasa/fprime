#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/EightyCharString.hpp>
#endif
#include <cstring>
namespace Fw {
// public methods

Buffer::Buffer(void): Serializable() {

}

Buffer::Buffer(const Buffer& src) : Serializable() {
    this->set(src.m_managerID, src.m_bufferID, src.m_data, src.m_size);
}

Buffer::Buffer(const Buffer* src) : Serializable() {
    FW_ASSERT(src);
    this->set(src->m_managerID, src->m_bufferID, src->m_data, src->m_size);
}

Buffer::Buffer(U32 managerID, U32 bufferID, U64 data, U32 size) : Serializable() {
    this->set(managerID, bufferID, data, size);
}

const Buffer& Buffer::operator=(const Buffer& src) {
    this->set(src.m_managerID, src.m_bufferID, src.m_data, src.m_size);
    return src;
}

bool Buffer::operator==(const Buffer& src) const {
    return (
        (src.m_managerID == this->m_managerID) &&
        (src.m_bufferID == this->m_bufferID) &&
        (src.m_data == this->m_data) &&
        (src.m_size == this->m_size) &&
        true);
}

void Buffer::set(U32 managerID, U32 bufferID, U64 data, U32 size) {
    this->m_managerID = managerID;
    this->m_bufferID = bufferID;
    this->m_data = data;
    this->m_size = size;
}

U32 Buffer::getmanagerID(void) {
    return this->m_managerID;
}

U32 Buffer::getbufferID(void) {
    return this->m_bufferID;
}

U64 Buffer::getdata(void) {
    return this->m_data;
}

U32 Buffer::getsize(void) {
    return this->m_size;
}

void Buffer::setmanagerID(U32 val) {
    this->m_managerID = val;
}
void Buffer::setbufferID(U32 val) {
    this->m_bufferID = val;
}
void Buffer::setdata(U64 val) {
    this->m_data = val;
}
void Buffer::setsize(U32 val) {
    this->m_size = val;
}
Fw::SerializeStatus Buffer::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    // serialize type ID
    stat = buffer.serialize((U32)Buffer::TYPE_ID);
#endif

    stat = buffer.serialize(this->m_managerID);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_bufferID);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_data);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_size);
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

    stat = buffer.deserialize(this->m_managerID);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_bufferID);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_data);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_size);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING  || BUILD_UT

void Buffer::toString(Fw::StringBase& text) const {

    static const char * formatString =
       "("
       "managerID = %u, "
       "bufferID = %u, "
       "data = %lu, "
       "size = %u"
       ")";

    // declare strings to hold any serializable toString() arguments


    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    (void)snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,formatString
       ,this->m_managerID
       ,this->m_bufferID
       ,this->m_data
       ,this->m_size
    );
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

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
