#include <Utils/Hash/HashBuffer.hpp>
#include <cstring>

namespace Utils {

    HashBuffer::HashBuffer() {
    }

    HashBuffer::HashBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    HashBuffer::~HashBuffer() {
    }

    HashBuffer::HashBuffer(const HashBuffer& other) : Fw::SerializeBufferBase() {
        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    HashBuffer& HashBuffer::operator=(const HashBuffer& other) {
        if(this == &other) {
            return *this;
        }

        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    bool HashBuffer::operator==(const HashBuffer& other) const {
        if( (this->getBuffLength() == other.getBuffLength()) &&
            (memcmp(this->getBuffAddr(), other.getBuffAddr(), this->getBuffLength()) != 0) ){
          return false;
        }
        return true;
    }

    bool HashBuffer::operator!=(const HashBuffer& other) const {
        return !(*this == other);
    }

    const U8* HashBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* HashBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

    NATIVE_UINT_TYPE HashBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }
}
