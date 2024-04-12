// ======================================================================
// @file   ExternalString.hpp
// @author Robert Bocchino
// @brief  A string backed by an external buffer
// ======================================================================

#ifndef FW_EXTERNAL_STRING_HPP
#define FW_EXTERNAL_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/StringType.hpp"
#include "Fw/Types/StringUtils.hpp"

namespace Fw {

//! A string backed by an external buffer
class ExternalString : public Fw::StringBase {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Constructor (bufferPtr and bufferSize)
    ExternalString(char* bufferPtr,                 //!< The buffer pointer
                   StringBase::SizeType bufferSize  //!< The buffer size
                   )
        : m_bufferPtr(bufferPtr), m_bufferSize(bufferSize) {}

    //! Constructor (bufferPtr, bufferSize, and StringBase)
    ExternalString(char* bufferPtr,                  //!< The buffer pointer
                   StringBase::SizeType bufferSize,  //!< The buffer size
                   const StringBase& sb              //!< The source string
                   )
        : m_bufferPtr(bufferPtr), m_bufferSize(bufferSize) {
        *this = sb;
    }

    //! Constructor (bufferPtr, bufferSize, and const char*)
    ExternalString(char* bufferPtr,                  //!< The buffer pointer
                   StringBase::SizeType bufferSize,  //!< The buffer size
                   const char* str                   //!< The source string
                   )
        : m_bufferPtr(bufferPtr), m_bufferSize(bufferSize) {
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
    const char* toChar() const { return this->m_bufferPtr; }

    //! Returns the buffer size
    //! @return The buffer size
    StringBase::SizeType getCapacity() const { return this->m_bufferSize; }

  public:
    // ----------------------------------------------------------------------
    // Operators
    // ----------------------------------------------------------------------

    // ExternalString assignment operator
    ExternalString& operator=(const ExternalString& sb  //!< The external string
    ) {
        if (this != &sb) {
            (void)Fw::StringUtils::string_copy(this->m_bufferPtr, sb.toChar(), this->m_bufferSize);
        }
        return *this;
    }

    // StringBase assignment operator
    ExternalString& operator=(const StringBase& sb  //!< The StringBase
    ) {
        if (this != &sb) {
            (void)Fw::StringUtils::string_copy(this->m_bufferPtr, sb.toChar(), this->m_bufferSize);
        }
        return *this;
    }

    // const char* assignment operator
    ExternalString& operator=(const char* str  //!< The C-style string
    ) {
        (void)Fw::StringUtils::string_copy(this->m_bufferPtr, str, this->m_bufferSize);
        return *this;
    }

  private:
    // ----------------------------------------------------------------------
    // Data members
    // ----------------------------------------------------------------------

    //! Pointer to string buffer
    char* m_bufferPtr;

    //! Size of string buffer
    //! F Prime strings are null-terminated, so this is one more than
    //! the length of the largest string that the buffer can hold
    StringBase::SizeType m_bufferSize;
};
}  // namespace Fw

#endif
