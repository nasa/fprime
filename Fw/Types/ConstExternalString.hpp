// ======================================================================
// @file   ConstExternalString.hpp
// @brief  A string backed by an immutable string literal
// ======================================================================

#ifndef FW_CONST_EXTERNAL_STRING_HPP
#define FW_CONST_EXTERNAL_STRING_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
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
    ConstExternalString(const char* bufferPtr,                //!< The buffer pointer
                        ConstStringBase::SizeType bufferSize  //!< The buffer size
                        )
        : ConstStringBase(), m_bufferPtr(bufferPtr), m_bufferSize(bufferSize) {
        FW_ASSERT(bufferPtr != nullptr);
        FW_ASSERT(bufferSize > 0, static_cast<FwAssertArgType>(bufferSize));
        // Assert that the string length plus the null terminator fills the buffer
        FW_ASSERT(ConstStringBase::length() + 1 == bufferSize,
                  static_cast<FwAssertArgType>(ConstStringBase::length() + 1),
                  static_cast<FwAssertArgType>(bufferSize));
    }

    //! Destructor
    ~ConstExternalString() {}

  public:
    // ----------------------------------------------------------------------
    // ConstStringBase interface
    // ----------------------------------------------------------------------

    //! Get the length of the string
    ConstStringBase::SizeType length() const override {
        // The length of the string is 1 less than its capacity (string + null terminator)
        return this->m_bufferSize == 0 ? 0 : this->m_bufferSize - 1;
    }

    //! Gets the char buffer
    const char* toChar() const override { return this->m_bufferPtr; }

    //! Returns the buffer size
    ConstStringBase::SizeType getCapacity() const override { return this->m_bufferSize; }

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
