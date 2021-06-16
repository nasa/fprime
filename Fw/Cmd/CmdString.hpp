#ifndef FW_CMD_STRING_TYPE_HPP
#define FW_CMD_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <FpConfig.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class CmdStringArg : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_CMD_STR,
                SERIALIZED_SIZE = FW_CMD_STRING_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            CmdStringArg(const char* src);
            CmdStringArg(const StringBase& src);
            CmdStringArg(const CmdStringArg& src);
            CmdStringArg(void);
            ~CmdStringArg(void);
            const char* toChar(void) const;
            NATIVE_UINT_TYPE length(void) const;

            const CmdStringArg& operator=(const CmdStringArg& other); //!< equal operator for other strings

            SerializeStatus serialize(SerializeBufferBase& buffer) const;
            SerializeStatus deserialize(SerializeBufferBase& buffer);

        private:
            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_CMD_STRING_MAX_SIZE];
    };

}

#endif
