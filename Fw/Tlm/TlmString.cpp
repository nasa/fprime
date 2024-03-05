#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    TlmString::TlmString(const char* src) :  StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    TlmString::TlmString(const StringBase& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TlmString::TlmString(const TlmString& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TlmString::TlmString() : StringBase() {
        this->m_buf[0] = 0;
    }

    TlmString& TlmString::operator=(const TlmString& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TlmString& TlmString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TlmString& TlmString::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    TlmString::~TlmString() {
    }

    const char* TlmString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE TlmString::getCapacity() const {
        return FW_TLM_STRING_MAX_SIZE;
    }

    SerializeStatus TlmString::serialize(SerializeBufferBase& buffer) const {
        return this->serialize(buffer, this->length());
    }

    SerializeStatus TlmString::serialize(SerializeBufferBase& buffer, NATIVE_UINT_TYPE maxLength) const {
        NATIVE_INT_TYPE len = FW_MIN(maxLength,this->length());
#if FW_AMPCS_COMPATIBLE
        // serialize 8-bit size with null terminator removed
        U8 strSize = len - 1;
        SerializeStatus stat = buffer.serialize(strSize);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize(reinterpret_cast<const U8*>(this->toChar()),strSize, true);
#else
        return buffer.serialize(reinterpret_cast<const U8*>(this->toChar()),len);
#endif
    }

    SerializeStatus TlmString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = this->getCapacity() - 1;
        CHAR* raw = const_cast<CHAR*>(this->toChar());

#if FW_AMPCS_COMPATIBLE
        // AMPCS encodes 8-bit string size
        U8 strSize;
        SerializeStatus stat = buffer.deserialize(strSize);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        strSize = FW_MIN(maxSize,strSize);
        stat = buffer.deserialize(reinterpret_cast<U8*>(raw),strSize,true);
        // AMPCS Strings not null terminated
        if(strSize < maxSize) {
            raw[strSize] = 0;
        }
#else
        SerializeStatus stat = buffer.deserialize(reinterpret_cast<U8*>(raw),maxSize);
#endif

        // Null terminate deserialized string
        raw[maxSize] = 0;
        return stat;
    }
}
