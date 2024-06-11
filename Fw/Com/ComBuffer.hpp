/*
 * FwComBuffer.hpp
 *
 *  Created on: May 24, 2014
 *      Author: tcanham
 */

/*
 * Description:
 * This object contains the ComBuffer type, used for sending and receiving packets from the ground
 */
#ifndef FW_COM_BUFFER_HPP
#define FW_COM_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

    class ComBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = 1010,
                SERIALIZED_SIZE = FW_COM_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)  // size of buffer + storage of size word
            };

            ComBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            ComBuffer();
            ComBuffer(const ComBuffer& other);
            virtual ~ComBuffer();
            ComBuffer& operator=(const ComBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        private:
            U8 m_bufferData[FW_COM_BUFFER_MAX_SIZE]; // packet data buffer
    };

}

#endif
