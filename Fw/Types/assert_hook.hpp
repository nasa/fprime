// ======================================================================
// \title  assert_hook.hpp
// \author Andrei Tumbar
// \brief  link-time selected default assert handling functions (Fw_AssertHook implementations)
//
// \copyright
// Copyright (C) 2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef FW_TYPES_ASSERT_HOOK_HPP_
#define FW_TYPES_ASSERT_HOOK_HPP_
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

//! \brief format an assert message
//!
//! Formats the location of a failed assertion and its arguments into destBuffer. Called by the
//! framework when no Fw::AssertHook is registered, and by Svc::AssertFatalAdapter.
//!
//! \param file: file name or file ID (depending on FW_ASSERT_LEVEL)
//! \param lineNo: line number where the assert occurred
//! \param numArgs: number of additional arguments (0-6)
//! \param arg1-arg6: optional assert arguments
//! \param destBuffer: buffer to write the formatted message to
//! \param buffSize: size of destBuffer
void formatAssertMessage(FILE_NAME_ARG file,
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

//! \brief print an assert message
//!
//! Outputs a message formatted by formatAssertMessage. Called by the framework when no
//! Fw::AssertHook is registered.
//!
//! \param msg: null-terminated string containing the formatted assert message
void printAssertMessage(const CHAR* msg);

//! \brief terminate after a failed assertion
//!
//! Called by the framework after printAssertMessage when no Fw::AssertHook is registered, and by
//! the default Fw::AssertHook::doAssert. Implementations typically call `assert(false)`, which is
//! compiled out when NDEBUG is defined; FW_ASSERTIONS_ALWAYS_ABORT then decides whether execution
//! may continue.
void terminateOnAssert();

}  // namespace Fw
#endif  // FW_TYPES_ASSERT_HOOK_HPP_
