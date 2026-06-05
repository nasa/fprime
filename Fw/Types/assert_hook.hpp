// ======================================================================
// \title  assert_hook.hpp
// \author Andrei Tumbar
// \brief  hpp file for assert hook functions
//
// \copyright
// Copyright (C) 2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef FW_TYPES_ASSERT_HOOKS_HPP_
#define FW_TYPES_ASSERT_HOOKS_HPP_
#include <Fw/FPrimeBasicTypes.hpp>

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define FILE_NAME_ARG U32
#else
#define FILE_NAME_ARG const CHAR*
#endif

namespace Fw {

//! \brief handles an assertion
//!
//! Handle an assertion failure or FATAL. This function is called after the defaultPrintAssert has
//! been called. The default implementation calls `assert(0)`.
[[noreturn]] void defaultDoAssert();

//! \brief print an assert message
//!
//! Print a pre-formatted assert message. This function is called after the assert message has been formatted
//! by defaultReportAssert. The default implementation uses fputs to write to stderr.
//!
//! \param msg: null-terminated string containing the formatted assert message
void defaultPrintAssert(const CHAR* msg);

//! \brief format an assert message
//!
//! Format an assert message with file, line number, and optional arguments. The formatted message is written
//! to destBuffer. This function is called to format assert messages before they are printed.
//!
//! \param file: file name or file ID (depending on FW_ASSERT_LEVEL)
//! \param lineNo: line number where the assert occurred
//! \param numArgs: number of additional arguments (0-6)
//! \param arg1-arg6: optional assert arguments
//! \param destBuffer: buffer to write formatted message
//! \param buffSize: size of destBuffer
void defaultReportAssert(FILE_NAME_ARG file,
                         FwSizeType lineNo,
                         FwSizeType numArgs,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwAssertArgType arg6,
                         CHAR* destBuffer,
                         FwSizeType buffSize);

}  // namespace Fw
#endif  // FW_TYPES_ASSERT_HOOKS_HPP_
