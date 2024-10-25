/*
 * TraceBuffer.hpp
 *
 *      Author: sreddy
 */

/*
 * Description:
 * This object contains the TraceBuffer type, used for storing trace entries
 */
#ifndef FW_TRACE_BUFFER_HPP
#define FW_TRACE_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Cfg/SerIds.hpp>
#include <string>

namespace Fw {

    class TraceBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_TRACE_BUFF,
                SERIALIZED_SIZE = FW_TRACE_BUFFER_MAX_SIZE+ sizeof(FwBuffSizeType)
            };

            TraceBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            TraceBuffer();
            TraceBuffer(const TraceBuffer& other);
            virtual ~TraceBuffer();
            TraceBuffer& operator=(const TraceBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;
            //! Supports writing this buffer to a string representation
            void toString(std::string& text) const;

        private:
            U8 m_bufferData[FW_TRACE_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
