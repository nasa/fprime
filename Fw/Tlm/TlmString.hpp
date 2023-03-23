#ifndef FW_TLM_STRING_TYPE_HPP
#define FW_TLM_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class TlmString : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_TLM_STR,
                SERIALIZED_SIZE = FW_TLM_STRING_MAX_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of size word
            };

            TlmString();
            TlmString(const TlmString& src); //!< TlmString string constructor
            TlmString(const StringBase& src); //!< other string constructor
            TlmString(const char* src); //!< char* source constructor
            TlmString& operator=(const TlmString& other); //!< assignment operator
            TlmString& operator=(const StringBase& other); //!< other string assignment operator
            TlmString& operator=(const char* other); //!< char* assignment operator
            ~TlmString();

            const char* toChar() const override;
            NATIVE_UINT_TYPE getCapacity() const override;

            SerializeStatus serialize(SerializeBufferBase& buffer) const override; //!< serialization function
            SerializeStatus serialize(SerializeBufferBase& buffer, NATIVE_UINT_TYPE maxLen) const override; //!< serialization function
            SerializeStatus deserialize(SerializeBufferBase& buffer) override; //!< deserialization function

        private:
            char m_buf[FW_TLM_STRING_MAX_SIZE];
    };
}

#endif
