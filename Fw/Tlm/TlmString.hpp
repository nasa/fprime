#ifndef FW_TLM_STRING_TYPE_HPP
#define FW_TLM_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <FpConfig.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class TlmString : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_TLM_STR,
                SERIALIZED_SIZE = FW_TLM_STRING_MAX_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of size word
            };

            TlmString(const char* src);
            TlmString(const StringBase& src);
            TlmString(const TlmString& src);
            TlmString(void);
            ~TlmString(void);
            const char* toChar(void) const;
            NATIVE_UINT_TYPE length(void) const;
            void setMaxSerialize(NATIVE_UINT_TYPE size); // limit amount serialized

            const TlmString& operator=(const TlmString& other); //!< equal operator for other strings

            SerializeStatus serialize(SerializeBufferBase& buffer) const;
            SerializeStatus deserialize(SerializeBufferBase& buffer);

#if FW_SERIALIZABLE_TO_STRING
            void toString(StringBase& text) const;
#endif
        PRIVATE:
            NATIVE_UINT_TYPE getCapacity(void) const ;
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_TLM_STRING_MAX_SIZE];
            NATIVE_UINT_TYPE m_maxSer;
    };

}

#endif
