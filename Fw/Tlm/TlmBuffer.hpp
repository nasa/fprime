/*
 * TlmBuffer.hpp
 *
 *      Author: tcanham
 */

/*
 * Description:
 * This object contains the TlmBuffer type, used for storing telemetry
 */
#ifndef FW_TLM_BUFFER_HPP
#define FW_TLM_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class TlmBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_TLM_BUFF,
                SERIALIZED_SIZE = FW_TLM_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            TlmBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            TlmBuffer();
            TlmBuffer(const TlmBuffer& other);
            virtual ~TlmBuffer();
            TlmBuffer& operator=(const TlmBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        PRIVATE:
            U8 m_bufferData[FW_TLM_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
