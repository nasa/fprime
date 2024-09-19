#include <Fw/Sm/SmSignalBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    SmSignalBuffer::SmSignalBuffer(const U8 *args, Serializable::SizeType size) : m_bufferData{} {
        FW_ASSERT(args != nullptr);
        FW_ASSERT(size <= sizeof(this->m_bufferData));
        SerializeStatus stat = SerializeBufferBase::setBuff(args,static_cast<NATIVE_UINT_TYPE>(size));
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SmSignalBuffer::SmSignalBuffer() : m_bufferData{} {
    }

    SmSignalBuffer::~SmSignalBuffer() {
    }

    SmSignalBuffer::SmSignalBuffer(const SmSignalBuffer& other) : Fw::SerializeBufferBase(),
                                                                   m_bufferData{}
   {
        FW_ASSERT(other.getBuffAddr() != nullptr);
        FW_ASSERT(other.getBuffLength() <= sizeof(this->m_bufferData));

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SmSignalBuffer& SmSignalBuffer::operator=(const SmSignalBuffer& other) {
        if(this == &other) {
            return *this;
        }
       
        FW_ASSERT(other.getBuffAddr() != nullptr);
        FW_ASSERT(other.getBuffLength() <= sizeof(this->m_bufferData));

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    Serializable::SizeType SmSignalBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* SmSignalBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* SmSignalBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}

