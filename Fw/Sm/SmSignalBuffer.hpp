/*
 * SmSignalBuffer.hpp
 *
 */

/*
 * Description:
 * This object contains the SmSignalBuffer type, used for attaching data to state machine signals
 */
#ifndef FW_SM_SIGNAL_BUFFER_HPP
#define FW_SM_SIGNAL_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

    class SmSignalBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = 1010,
                SERIALIZED_SIZE = FW_COM_BUFFER_MAX_SIZE + sizeof(FwSizeStoreType)  // size of buffer + storage of size word
            };

            SmSignalBuffer(const U8 *args, Serializable::SizeType size);
            SmSignalBuffer();
            SmSignalBuffer(const SmSignalBuffer& other);
            virtual ~SmSignalBuffer();
            SmSignalBuffer& operator=(const SmSignalBuffer& other);

            Serializable::SizeType getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        private:
            U8 m_bufferData[FW_SM_SIGNAL_BUFFER_MAX_SIZE]; // packet data buffer
    };

}

#endif
