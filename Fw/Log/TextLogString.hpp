#ifndef FW_TEXT_LOG_STRING_TYPE_HPP
#define FW_TEXT_LOG_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class TextLogString : public Fw::StringBase {
        public:
        
            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
                SERIALIZED_SIZE = FW_LOG_TEXT_BUFFER_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of two size words
            };
        
            TextLogString(const char* src);
            TextLogString(const StringBase& src);
            TextLogString(const TextLogString& src);
            TextLogString(void);
            ~TextLogString(void);
            const char* toChar(void) const;
            NATIVE_UINT_TYPE length(void) const;
            
            const TextLogString& operator=(const TextLogString& other); //!< equal operator for other strings

            SerializeStatus serialize(SerializeBufferBase& buffer) const;
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            
        private:
            void copyBuff(const char* buff, NATIVE_UINT_TYPE size);
            NATIVE_UINT_TYPE getCapacity(void) const ;
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_LOG_TEXT_BUFFER_SIZE];
    };

}

#endif
