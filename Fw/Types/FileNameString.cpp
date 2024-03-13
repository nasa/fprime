#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/StringUtils.hpp"

namespace Fw {

    FileNameString::FileNameString(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    FileNameString::FileNameString(const StringBase& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    FileNameString::FileNameString(const FileNameString& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    FileNameString::FileNameString() : StringBase() {
        this->m_buf[0] = 0;
    }

    FileNameString& FileNameString::operator=(const FileNameString& other) {
        if(this == &other) {
            return *this;
        }

        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    FileNameString& FileNameString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    FileNameString& FileNameString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    FileNameString::~FileNameString() {
    }

    const char* FileNameString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE FileNameString::getCapacity() const {
        return STRING_SIZE;
    }
}
