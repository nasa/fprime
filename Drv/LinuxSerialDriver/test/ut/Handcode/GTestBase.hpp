// ======================================================================
// \title  LinuxSerialDriver/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxSerialDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxSerialDriver_GTEST_BASE_HPP
#define LinuxSerialDriver_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_DR_BytesSent_SIZE(size) \
  this->assertTlm_DR_BytesSent_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_DR_BytesSent(index, value) \
  this->assertTlm_DR_BytesSent(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_DR_BytesRecv_SIZE(size) \
  this->assertTlm_DR_BytesRecv_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_DR_BytesRecv(index, value) \
  this->assertTlm_DR_BytesRecv(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_OpenError_SIZE(size) \
  this->assertEvents_DR_OpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_OpenError(index, _device, _error) \
  this->assertEvents_DR_OpenError(__FILE__, __LINE__, index, _device, _error)

#define ASSERT_EVENTS_DR_ConfigError_SIZE(size) \
  this->assertEvents_DR_ConfigError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_ConfigError(index, _device, _error) \
  this->assertEvents_DR_ConfigError(__FILE__, __LINE__, index, _device, _error)

#define ASSERT_EVENTS_DR_WriteError_SIZE(size) \
  this->assertEvents_DR_WriteError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_WriteError(index, _device, _error) \
  this->assertEvents_DR_WriteError(__FILE__, __LINE__, index, _device, _error)

#define ASSERT_EVENTS_DR_ReadError_SIZE(size) \
  this->assertEvents_DR_ReadError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_ReadError(index, _device, _error) \
  this->assertEvents_DR_ReadError(__FILE__, __LINE__, index, _device, _error)

#define ASSERT_EVENTS_DR_PortOpened_SIZE(size) \
  this->assertEvents_DR_PortOpened_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_PortOpened(index, _device) \
  this->assertEvents_DR_PortOpened(__FILE__, __LINE__, index, _device)

#define ASSERT_EVENTS_DR_NoBuffers_SIZE(size) \
  this->assertEvents_DR_NoBuffers_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_NoBuffers(index, _device) \
  this->assertEvents_DR_NoBuffers(__FILE__, __LINE__, index, _device)

#define ASSERT_EVENTS_DR_BufferTooSmall_SIZE(size) \
  this->assertEvents_DR_BufferTooSmall_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DR_BufferTooSmall(index, _device, _size, _needed) \
  this->assertEvents_DR_BufferTooSmall(__FILE__, __LINE__, index, _device, _size, _needed)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_serialRecv_SIZE(size) \
  this->assert_from_serialRecv_size(__FILE__, __LINE__, size)

#define ASSERT_from_serialRecv(index, _serBuffer, _status) \
  { \
    ASSERT_GT(this->fromPortHistory_serialRecv->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_serialRecv\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_serialRecv->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_serialRecv& _e = \
      this->fromPortHistory_serialRecv->at(index); \
    ASSERT_EQ(_serBuffer, _e.serBuffer) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument serBuffer at index " \
    << index \
    << " in history of from_serialRecv\n" \
    << "  Expected: " << _serBuffer << "\n" \
    << "  Actual:   " << _e.serBuffer << "\n"; \
    ASSERT_EQ(_status, _e.status) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument status at index " \
    << index \
    << " in history of from_serialRecv\n" \
    << "  Expected: " << _status << "\n" \
    << "  Actual:   " << _e.status << "\n"; \
  }

namespace Drv {

  //! \class LinuxSerialDriverGTestBase
  //! \brief Auto-generated base class for LinuxSerialDriver component Google Test harness
  //!
  class LinuxSerialDriverGTestBase :
    public LinuxSerialDriverTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxSerialDriverGTestBase
      //!
      LinuxSerialDriverGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxSerialDriverGTestBase
      //!
      virtual ~LinuxSerialDriverGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: DR_BytesSent
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_DR_BytesSent_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_DR_BytesSent(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: DR_BytesRecv
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_DR_BytesRecv_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_DR_BytesRecv(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_OpenError
      // ----------------------------------------------------------------------

      void assertEvents_DR_OpenError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_OpenError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_ConfigError
      // ----------------------------------------------------------------------

      void assertEvents_DR_ConfigError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_ConfigError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_WriteError
      // ----------------------------------------------------------------------

      void assertEvents_DR_WriteError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_WriteError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_ReadError
      // ----------------------------------------------------------------------

      void assertEvents_DR_ReadError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_ReadError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_PortOpened
      // ----------------------------------------------------------------------

      void assertEvents_DR_PortOpened_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_PortOpened(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device /*!< The device*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_NoBuffers
      // ----------------------------------------------------------------------

      void assertEvents_DR_NoBuffers_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_NoBuffers(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device /*!< The device*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_BufferTooSmall
      // ----------------------------------------------------------------------

      void assertEvents_DR_BufferTooSmall_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DR_BufferTooSmall(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 device, /*!< The device*/
          const U32 size, /*!< The provided buffer size*/
          const U32 needed /*!< The buffer size needed*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From ports 
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: serialRecv 
      // ----------------------------------------------------------------------

      void assert_from_serialRecv_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Drv

#endif
