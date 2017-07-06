#include <Fw/ComFile/FileBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

	FileBuffer::FileBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
	    SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }
    
	FileBuffer::FileBuffer() {
    }

	FileBuffer::~FileBuffer() {
    }

	FileBuffer::FileBuffer(const FileBuffer& other) : Fw::SerializeBufferBase() {
	    SerializeStatus stat = this->setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
	}

	const FileBuffer& FileBuffer::operator=(const FileBuffer& other) {
	    SerializeStatus stat = this->setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return other;
	}

    NATIVE_UINT_TYPE FileBuffer::getBuffCapacity(void) const {
        return sizeof(this->m_data);
    }

    const U8* FileBuffer::getBuffAddr(void) const {
        return this->m_data;
    }

    U8* FileBuffer::getBuffAddr(void) {
        return this->m_data;
    }


}

