/**
 * \file StringBase.hpp
 * \author T. Canham
 * \brief Declares F Prime string base class
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef FW_STRING_BASE_HPP
#define FW_STRING_BASE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <cstdarg>
#ifdef BUILD_UT
#include <iostream>
#endif

namespace Fw {
class StringBase : public Serializable {
  public:
    using SizeType = NATIVE_UINT_TYPE;
    virtual const CHAR* toChar() const = 0;    //<! Convert to a C-style char*
    virtual SizeType getCapacity() const = 0;  //!< return size of buffer
    SizeType length() const;                   //!< Get length of string

    //! Get the maximum length of a string that the buffer can hold (which is capacity - 1)
    SizeType maxLength() const;
    //! Get the static serialized size of a string
    //! This is the max length of the string plus the size of the stored size
    static constexpr SizeType STATIC_SERIALIZED_SIZE(SizeType maxLength  //!< The maximum string length
    ) {
        return static_cast<SizeType>(sizeof(FwSizeStoreType)) + maxLength;
    }

    //! Get the size of a null-terminated string buffer
    static constexpr SizeType BUFFER_SIZE(SizeType maxLength  //!< The maximum string length
    ) {
        // Reserve one byte for each character plus one for the null terminator
        return maxLength + 1;
    }

    //! Get the dynamic serialized size of a string
    //! This is the length of the string plus the size of the stored size
    SizeType serializedSize() const;

    //! Get the serialized truncated size of a string
    //! This is the minimum of the dynamic serialized size and the max length
    SizeType serializedTruncatedSize(FwSizeType maxLength  //!< The max string length
    ) const;

    const CHAR* operator+=(const CHAR* src);              //!< Concatenate a CHAR*
    const StringBase& operator+=(const StringBase& src);  //!< Concatenate a StringBase
    bool operator==(const StringBase& other) const;       //!< Check for equality with StringBase
    bool operator==(const CHAR* other) const;             //!< Check for equality with CHAR*
    bool operator!=(const StringBase& other) const;       //!< Inequality with StringBase
    bool operator!=(const CHAR* other) const;             //!< Inequality with CHAR*
    StringBase& operator=(const CHAR* src);               //!< Assign CHAR*
    StringBase& operator=(const StringBase& src);         //!< Assign another StringBase

    void format(const CHAR* formatString, ...);  //!< write formatted string to buffer
    void vformat(const CHAR* formatString, va_list args);  //!< write formatted string to buffer using va_list

    virtual SerializeStatus serialize(SerializeBufferBase& buffer) const;                   //!< serialization function
    virtual SerializeStatus serialize(SerializeBufferBase& buffer, SizeType maxLen) const;  //!< serialization function
    virtual SerializeStatus deserialize(SerializeBufferBase& buffer);  //!< deserialization function

#ifdef BUILD_UT
    // to support GoogleTest framework in unit tests
    friend std::ostream& operator<<(std::ostream& os, const StringBase& str);
#endif
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(StringBase& text) const;  //!< write string with contents
#endif

  protected:
    StringBase();
    virtual ~StringBase();

    void appendBuff(const CHAR* buff, SizeType size);

  private:
    StringBase(const StringBase& src) = delete;  //!< constructor with buffer as source
};

}  // namespace Fw

#endif
