#ifndef FW_LOG_STRING_TYPE_HPP
#define FW_LOG_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <FpConfig.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class LogStringArg : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
                SERIALIZED_SIZE = FW_LOG_STRING_MAX_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of two size words
            };

            LogStringArg(const char* src);
            LogStringArg(const StringBase& src);
            LogStringArg(const LogStringArg& src);
            LogStringArg(void);
            ~LogStringArg(void);
            const char* toChar(void) const;
            NATIVE_UINT_TYPE length(void) const;
            // This method is set by the autocode to the max length specified in the XML declaration for a particular event.
            void setMaxSerialize(NATIVE_UINT_TYPE size); // limit amount serialized

            const LogStringArg& operator=(const LogStringArg& other); //!< equal operator for other strings

            SerializeStatus serialize(SerializeBufferBase& buffer) const;
            SerializeStatus deserialize(SerializeBufferBase& buffer);
#if FW_SERIALIZABLE_TO_STRING
            void toString(StringBase& text) const;
#endif

        private:

            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_LOG_STRING_MAX_SIZE];
            NATIVE_UINT_TYPE m_maxSer;
    };

}

#endif
