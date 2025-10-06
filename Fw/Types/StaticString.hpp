// ======================================================================
// @file   StaticString.hpp
// @brief  A string backed by an immutable string literal
// ======================================================================

#ifndef FW_STATIC_STRING_HPP
#define FW_STATIC_STRING_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/StringBase.hpp>

namespace Fw {

//! A string backed by an immutable string literal
class StaticString final : public Fw::StringBase {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Constructor (uninitialized buffer)
    StaticString() : StringBase(), m_bufferPtr(nullptr), m_bufferSize(0) {}

    //! Constructor (bufferPtr and bufferSize)
    StaticString(const char* bufferPtr,           //!< The buffer pointer
                 StringBase::SizeType bufferSize  //!< The buffer size
                 )
        : StringBase(), m_bufferPtr(bufferPtr), m_bufferSize(bufferSize + 1) {}

    //! Destructor
    ~StaticString() {}

  public:
    // ----------------------------------------------------------------------
    // StringBase interface
    // ----------------------------------------------------------------------

    //! Gets the char buffer
    const char* toChar() const { return this->m_bufferPtr; }

    //! Returns the buffer size
    StringBase::SizeType getCapacity() const { return this->m_bufferSize; }

  private:
    // ----------------------------------------------------------------------
    // Data members
    // ----------------------------------------------------------------------

    //! Pointer to string buffer
    const char* m_bufferPtr;
    //! Size of string buffer
    //! F Prime strings are null-terminated, so this is one more than
    //! the length of the largest string that the buffer can hold
    StringBase::SizeType m_bufferSize;
};

}  // namespace Fw

#endif
