#ifndef _DrvDataBuffer_hpp_
#define _DrvDataBuffer_hpp_

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Drv {

    class DataBuffer : public Fw::SerializeBufferBase {
        public:

            enum {
                DATA_BUFFER_SIZE = 256,
                SERIALIZED_TYPE_ID = 1010,
                SERIALIZED_SIZE = DATA_BUFFER_SIZE + sizeof(FwBuffSizeType)
            };

            DataBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            DataBuffer();
            DataBuffer(const DataBuffer& other);
            virtual ~DataBuffer();
            DataBuffer& operator=(const DataBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        private:
            U8 m_data[DATA_BUFFER_SIZE]; // packet data buffer
    };
}

#endif
