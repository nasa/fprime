// ======================================================================
// \title  format.hpp
// \author mstarch
// \brief  hpp file for c-string format function
//
// \copyright
// Copyright (C) 2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef FW_TYPES_FORMAT_HPP_
#define FW_TYPES_FORMAT_HPP_
#include <FpConfig.h>
#include <cstdarg>
namespace Fw {

//! \brief status of string format calls
enum class FormatStatus {
    SUCCESS, //!< Format worked
    OVERFLOWED, //!< Format overflowed
    INVALID_FORMAT_STRING, //!< Format provided invalid format string
    SIZE_OVERFLOW, //!< FwSizeType overflowed the range of size_t
    OTHER_ERROR //!< An error was returned from an underlying call
};

//! \brief format a c-string
//!
//! Format a string using printf family formatting semantics. Destination will be filled with the formatted string up to
//! maximumSize - 1. This function will always terminate the string with a \0.
//!
//! This function can return several error codes:
//!   OVERFLOWED: the complete string did not fit in the buffer with an appended null-terminator
//!   INVALID_FORMAT_STRING: the format string was null
//!   OTHER_ERROR: another error occurred in an underlying function call
//! Otherwise SUCCESS is returned.  destination may be modified even in the case of an error.
//!
//! \param destination: destination to fill with the formatted string
//! \param maximumSize: size of the buffer represented by destination
//! \param formatString: format string to fill
//! \param ...: variable arguments inputs
//! \return: SUCCESS on successful formatting, OVERFLOWED on overflow, and something else on any error
    FormatStatus stringFormat(char* destination, const FwSizeType maximumSize, const char* formatString, ...);

//! \brief format a c-string
//!
//! Format a string using printf family formatting semantics. Destination will be filled with the formatted string up to
//! maximumSize - 1. This function will always terminate the string with a \0.
//!
//! This function can return several error codes:
//!   OVERFLOWED: the complete string did not fit in the buffer with an appended null-terminator
//!   INVALID_FORMAT_STRING: the format string was null
//!   OTHER_ERROR: another error occurred in an underlying function call
//! Otherwise SUCCESS is returned.  destination may be modified even in the case of an error.
//!
//! This version take a variable argument list
//!
//! \param destination: destination to fill with the formatted string
//! \param maximumSize: size of the buffer represented by destination
//! \param formatString: format string to fill
//! \param args: variable arguments list
//! \return: SUCCESS on successful formatting, OVERFLOWED on overflow, and something else on any error
FormatStatus stringFormat(char* destination, const FwSizeType maximumSize, const char* formatString, va_list args);
}
#endif // FW_TYPES_FORMAT_HPP_


