#ifndef FW_LOG_STRING_TYPE_HPP
#define FW_LOG_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class LogStringArg : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
                SERIALIZED_SIZE = FW_LOG_STRING_MAX_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of two size words
            };

            LogStringArg();
            LogStringArg(const LogStringArg& src); //!< LogStringArg string constructor
            LogStringArg(const StringBase& src); //!< other string constructor
            LogStringArg(const char* src); //!< char* source constructor
            LogStringArg& operator=(const LogStringArg& other); //!< assignment operator
            LogStringArg& operator=(const StringBase& other); //!< other string assignment operator
            LogStringArg& operator=(const char* other); //!< char* assignment operator
            ~LogStringArg();

            const char* toChar() const override;
            NATIVE_UINT_TYPE getCapacity() const override;

            SerializeStatus serialize(SerializeBufferBase& buffer) const override; //!< serialization function
            SerializeStatus serialize(SerializeBufferBase& buffer, NATIVE_UINT_TYPE maxLen) const override; //!< serialization function
            SerializeStatus deserialize(SerializeBufferBase& buffer) override; //!< deserialization function

        private:

            char m_buf[FW_LOG_STRING_MAX_SIZE];
    };

}

#endif
