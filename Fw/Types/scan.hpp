// ======================================================================
// \title  scan.hpp
// \author mstarch
// \brief  hpp file for c-string scan function (sscanf equivalent)
//
// \copyright
// Copyright (C) 2026 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef FW_TYPES_SCAN_HPP_
#define FW_TYPES_SCAN_HPP_
#include <Fw/FPrimeBasicTypes.hpp>
#include <cstdarg>
namespace Fw {

//! \brief status of string scan calls
enum class ScanStatus {
    SUCCESS,                     //!< Scan worked
    INVALID_FORMAT_STRING,       //!< Format provided invalid format string
    SIZE_OVERFLOW,               //!< FwSizeType overflowed the range of size_t
    UNTERMINATED_SOURCE_STRING,  //!< The source string was not null-terminated within the maximum size
    OTHER_ERROR                  //!< An error was returned from an underlying call
};

//! \brief scan a c-string
//!
//! Scan a string using sscanf family formatting semantics. Source will be read according to the format string. The
//! number of filled fields will be returned in count. Read characters will not exceed maximumSize.
//!
//! This function can return several error codes:
//!   INVALID_FORMAT_STRING: the format string was null
//!   SIZE_OVERFLOW: FwSizeType overflowed the range of size_t
//!   OTHER_ERROR: another error occurred in an underlying function call
//! Otherwise SUCCESS is returned.
//!
//! \param count: [output] number of filled fields
//! \param source: source string to scan
//! \param maximumSize: size of the buffer represented by source
//! \param formatString: format string to fill
//! \param ...: variable arguments inputs
//! \return: SUCCESS on successful formatting, SIZE_OVERFLOW on overflow, and something else on any error
ScanStatus stringScan(FwSizeType& count, const char* source, FwSizeType maximumSize, const char* formatString, ...);

//! \brief scan a c-string using a variable argument list
//!
//! Scan a string using sscanf family formatting semantics. Source will be read according to the format string. The
//! number of filled fields will be returned in count. Read characters will not exceed maximumSize.
//!
//! This function can return several error codes:
//!   INVALID_FORMAT_STRING: the format string was null
//!   SIZE_OVERFLOW: FwSizeType overflowed the range of size_t
//!   OTHER_ERROR: another error occurred in an underlying function call
//! Otherwise SUCCESS is returned.
//!
//! This version takes a variable argument list
//!
//! \param count: [output] number of filled fields
//! \param source: source string to scan
//! \param maximumSize: size of the buffer represented by source
//! \param formatString: format string to fill
//! \param args: variable arguments list
//! \return: SUCCESS on successful formatting, SIZE_OVERFLOW on overflow, and something else on any error
ScanStatus stringScan(FwSizeType& count,
                      const char* source,
                      FwSizeType maximumSize,
                      const char* formatString,
                      va_list args);
}  // namespace Fw
#endif  // FW_TYPES_SCAN_HPP_
