// ======================================================================
// @file   ExternalString.hpp
// @author Robert Bocchino
// @brief  A string with an external buffer
// ======================================================================

#ifndef FW_EXTERNAL_STRING_HPP
#define FW_EXTERNAL_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/StringType.hpp"
#include "Fw/Types/StringUtils.hpp"

namespace Fw {

class ExternalString : public Fw::StringBase {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Constructor
    ExternalString(char* data, StringBase::SizeType capacity) : m_data(data), m_capacity(capacity) {}

    //! Constructor (data, capacity, and StringBase)
    ExternalString(char* data, StringBase::SizeType capacity, const StringBase& sb)
        : m_data(data), m_capacity(capacity) {
        *this = sb;
    }

    //! Constructor (data, capacity, and char*)
    ExternalString(char* data, StringBase::SizeType capacity, const char* str) : m_data(data), m_capacity(capacity) {
        *this = str;
    }

    //! Destructor
    ~ExternalString() {}

  public:
    // ----------------------------------------------------------------------
    // StringBase interface
    // ----------------------------------------------------------------------

    //! Gets the char buffer
    //! @return The char buffer
    const char* toChar() const { return this->m_data; }

    //! Returns the buffer size
    //! @return The buffer size
    StringBase::SizeType getCapacity() const { return this->m_capacity; }

  public:
    // ----------------------------------------------------------------------
    // Operators
    // ----------------------------------------------------------------------

    // ExternalString assignment operator
    ExternalString& operator=(const ExternalString& sb) {
        if (this != &sb) {
            (void)Fw::StringUtils::string_copy(this->m_data, sb.toChar(), this->m_capacity);
        }
        return *this;
    }

    // StringBase assignment operator
    ExternalString& operator=(const StringBase& sb) {
        if (this != &sb) {
            (void)Fw::StringUtils::string_copy(this->m_data, sb.toChar(), this->m_capacity);
        }
        return *this;
    }

    // const char* assignment operator
    ExternalString& operator=(const char* str) {
        (void)Fw::StringUtils::string_copy(this->m_data, str, this->m_capacity);
        return *this;
    }

  private:
    // ----------------------------------------------------------------------
    // Member data
    // ----------------------------------------------------------------------

    //! Storage for string data
    char* m_data;

    //! Size of string data
    StringBase::SizeType m_capacity;
};
}  // namespace Fw

#endif
