#include <Fw/Trace/TraceBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <cstdio>

namespace Fw {

    TraceBuffer::TraceBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        FW_ASSERT(args != nullptr);
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

    void TraceBuffer::toString(std::string& text) const{
        
        //FW_ASSERT(text != nullptr);
        FW_ASSERT(this->getBuffLength() < FW_TRACE_BUFFER_MAX_SIZE);

        std::string str_format(reinterpret_cast<const char*>(this->m_bufferData),this->getBuffLength());
        text += str_format;
    }
}
