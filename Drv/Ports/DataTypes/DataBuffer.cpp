#include <Drv/Ports/DataTypes/DataBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Drv {

    DataBuffer::DataBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    DataBuffer::DataBuffer() {
    }

    DataBuffer::~DataBuffer() {
    }

    DataBuffer::DataBuffer(const DataBuffer& other) : Fw::SerializeBufferBase() {
        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    DataBuffer& DataBuffer::operator=(const DataBuffer& other) {
        if(this == &other) {
            return *this;
        }

        Fw::SerializeStatus stat = Fw::SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE DataBuffer::getBuffCapacity() const {
        return sizeof(this->m_data);
    }

    const U8* DataBuffer::getBuffAddr() const {
        return this->m_data;
    }

    U8* DataBuffer::getBuffAddr() {
        return this->m_data;
    }

}
