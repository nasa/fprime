#include <Fw/Prm/PrmBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

	ParamBuffer::ParamBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
	    SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }
    
	ParamBuffer::ParamBuffer() {
    }

    ParamBuffer::~ParamBuffer() {
    }

    ParamBuffer::ParamBuffer(const ParamBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
	}

	const ParamBuffer& ParamBuffer::operator=(const ParamBuffer& other) {
	    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return other;
	}

    NATIVE_UINT_TYPE ParamBuffer::getBuffCapacity(void) const {
        return sizeof(this->m_data);
    }

    const U8* ParamBuffer::getBuffAddr(void) const {
        return this->m_data;
    }

    U8* ParamBuffer::getBuffAddr(void) {
        return this->m_data;
    }

}

