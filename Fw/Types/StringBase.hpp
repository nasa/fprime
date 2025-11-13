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
#include <Fw/Types/ConstStringBase.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/format.hpp>
#include <cstdarg>
#ifdef BUILD_UT
#include <iostream>
#endif

namespace Fw {

class StringBase : public ConstStringBase {
  public:
    const CHAR* operator+=(const CHAR* src);                   //!< Concatenate a CHAR*
    const StringBase& operator+=(const ConstStringBase& src);  //!< Concatenate a ConstStringBase
    const StringBase& operator+=(const StringBase& src);       //!< Concatenate a StringBase

    StringBase& operator=(const CHAR* src);             //!< Assign CHAR*
    StringBase& operator=(const ConstStringBase& src);  //!< Assign another ConstStringBase
    StringBase& operator=(const StringBase& src);       //!< Assign another StringBase

    FormatStatus format(const CHAR* formatString, ...);            //!< write formatted string to buffer
    FormatStatus vformat(const CHAR* formatString, va_list args);  //!< write formatted string to buffer using va_list

    SerializeStatus deserializeFrom(SerialBufferBase& buffer, Endianness mode = Endianness::BIG) override;

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
