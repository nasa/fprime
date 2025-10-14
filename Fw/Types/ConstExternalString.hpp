// ======================================================================
// @file   ConstExternalString.hpp
// @brief  A string backed by an immutable string literal
// ======================================================================

#ifndef FW_CONST_EXTERNAL_STRING_HPP
#define FW_CONST_EXTERNAL_STRING_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/StringBase.hpp>

namespace Fw {

//! A string backed by an immutable string literal
class ConstExternalString final : public ConstStringBase {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Constructor (uninitialized buffer)
    ConstExternalString() : ConstStringBase(), m_bufferPtr(nullptr), m_bufferSize(0) {}

    //! Constructor (bufferPtr and bufferSize)
    ConstExternalString(const char* bufferPtr,           //!< The buffer pointer
                        ConstStringBase::SizeType bufferSize  //!< The buffer size
                        )
        : ConstStringBase(), m_bufferPtr(bufferPtr), m_bufferSize(bufferSize + 1) {}

    //! Destructor
    ~ConstExternalString() {}

  public:
    // ----------------------------------------------------------------------
    // ConstStringBase interface
    // ----------------------------------------------------------------------

    //! Gets the char buffer
    const char* toChar() const { return this->m_bufferPtr; }

    //! Returns the buffer size
    ConstStringBase::SizeType getCapacity() const { return this->m_bufferSize; }

    //!< Get the length of the string
    ConstStringBase::SizeType length() const { return this->m_bufferSize - 1; }

  private:
    // ----------------------------------------------------------------------
    // Data members
    // ----------------------------------------------------------------------

    //! Pointer to string buffer
    const char* m_bufferPtr;
    //! Size of string buffer
    //! F Prime strings are null-terminated, so this is one more than
    //! the length of the largest string that the buffer can hold
    ConstStringBase::SizeType m_bufferSize;
};

}  // namespace Fw

#endif
