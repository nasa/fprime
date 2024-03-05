#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    CmdStringArg::CmdStringArg(const char* src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg(const StringBase& src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg(const CmdStringArg& src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    CmdStringArg::CmdStringArg() : StringBase() {
        this->m_buf[0] = 0;
    }

    CmdStringArg& CmdStringArg::operator=(const CmdStringArg& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg& CmdStringArg::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg& CmdStringArg::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    CmdStringArg::~CmdStringArg() {
    }

    const char* CmdStringArg::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE CmdStringArg::getCapacity() const {
        return FW_CMD_STRING_MAX_SIZE;
    }
}
