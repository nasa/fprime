#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    CmdStringArg::CmdStringArg(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg(const StringBase& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg(const CmdStringArg& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg(void) : StringBase() {
        this->m_buf[0] = 0;
    }

    CmdStringArg& CmdStringArg::operator=(const CmdStringArg& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg& CmdStringArg::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg& CmdStringArg::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg::~CmdStringArg(void) {
    }

    const char* CmdStringArg::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE CmdStringArg::getCapacity(void) const {
        return FW_CMD_STRING_MAX_SIZE;
    }
}
