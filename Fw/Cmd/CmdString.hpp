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
            CmdStringArg& operator=(const CmdStringArg& other);
            CmdStringArg& operator=(const StringBase& other);
            CmdStringArg& operator=(const char* other);
            ~CmdStringArg(void);

            const char* toChar(void) const;
            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size

        private:

            char m_buf[FW_CMD_STRING_MAX_SIZE];
    };

}

#endif
