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

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/format.hpp>
#include <cstdarg>
#ifdef BUILD_UT
#include <iostream>
#endif

namespace Fw {

//! A read-only abstract superclass for StringBase
class ConstStringBase : public Serializable {
  public:
    using SizeType = FwSizeType;

    //! Convert to a C-style char*
    virtual const CHAR* toChar() const = 0;
    //! Return the size of the buffer
    virtual SizeType getCapacity() const = 0;
    //! Get the length of the string
    virtual SizeType length() const;
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

    bool operator==(const ConstStringBase& other) const;  //!< Check for equality with ConstStringBase
    bool operator==(const CHAR* other) const;             //!< Check for equality with CHAR*
    bool operator!=(const ConstStringBase& other) const;  //!< Inequality with ConstStringBase
    bool operator!=(const CHAR* other) const;             //!< Inequality with CHAR*

    SerializeStatus serializeTo(SerializeBufferBase& buffer, Endianness mode = Endianness::BIG) const override;
    virtual SerializeStatus serializeTo(SerializeBufferBase& buffer,
                                        SizeType maxLen,
                                        Endianness mode = Endianness::BIG) const;
    // NOTE: all derived classes should override this function; it will always return an error
    // status since ConstStringBase is immutable and deserializeFrom is not a read-only operation
    SerializeStatus deserializeFrom(SerializeBufferBase& buffer, Endianness mode = Endianness::BIG) override;

    DEPRECATED(SerializeStatus serialize(SerializeBufferBase& buffer) const,
               "Use serializeTo(SerializeBufferBase& buffer) instead") {
        return this->serializeTo(buffer);
    }

    DEPRECATED(SerializeStatus serialize(SerializeBufferBase& buffer, SizeType maxLen) const,
               "Use serializeTo(SerializeBufferBase& buffer, SizeType maxLen) instead") {
        return this->serializeTo(buffer, maxLen);
    }

#ifdef BUILD_UT
    // to support GoogleTest framework in unit tests
    friend std::ostream& operator<<(std::ostream& os, const ConstStringBase& str);
#endif

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(StringBase& text) const override;  //!< write string with contents
#endif

  protected:
    ConstStringBase();
    virtual ~ConstStringBase();

  private:
    //! Deleted copy constructor
    ConstStringBase(const ConstStringBase& src) = delete;
};

class StringBase : public ConstStringBase {
  public:
    //! Get the length of the string
    SizeType length() const override;

    const CHAR* operator+=(const CHAR* src);                   //!< Concatenate a CHAR*
    const StringBase& operator+=(const ConstStringBase& src);  //!< Concatenate a ConstStringBase
    const StringBase& operator+=(const StringBase& src);       //!< Concatenate a StringBase

    StringBase& operator=(const CHAR* src);                    //!< Assign CHAR*
    StringBase& operator=(const ConstStringBase& src);         //!< Assign another ConstStringBase
    StringBase& operator=(const StringBase& src);              //!< Assign another StringBase

    FormatStatus format(const CHAR* formatString, ...);            //!< write formatted string to buffer
    FormatStatus vformat(const CHAR* formatString, va_list args);  //!< write formatted string to buffer using va_list

    SerializeStatus deserializeFrom(SerializeBufferBase& buffer, Endianness mode = Endianness::BIG) override;

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(StringBase& text) const override;  //!< write string with contents
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
