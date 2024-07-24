#include <Fw/Trace/TraceBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TraceBuffer::TraceBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    TraceBuffer::TraceBuffer() {
    }

    TraceBuffer::~TraceBuffer() {
    }

    TraceBuffer::TraceBuffer(const TraceBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    TraceBuffer& TraceBuffer::operator=(const TraceBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE TraceBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* TraceBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* TraceBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}
