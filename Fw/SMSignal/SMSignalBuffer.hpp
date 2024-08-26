/*
 * SMSignalBuffer.hpp
 *
 */

/*
 * Description:
 * This object contains the SMSignalBuffer type, used for attaching data to state machine signals
 */
#ifndef FW_SM_SIGNAL_BUFFER_HPP
#define FW_SM_SIGNAL_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

    class SMSignalBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = 1010,
                SERIALIZED_SIZE = FW_COM_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)  // size of buffer + storage of size word
            };

            SMSignalBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            SMSignalBuffer();
            SMSignalBuffer(const SMSignalBuffer& other);
            virtual ~SMSignalBuffer();
            SMSignalBuffer& operator=(const SMSignalBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        private:
            U8 m_bufferData[FW_SM_SIGNALS_BUFFER_MAX_SIZE]; // packet data buffer
    };

}

#endif
