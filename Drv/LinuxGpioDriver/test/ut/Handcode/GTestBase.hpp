// ======================================================================
// \title  LinuxGpioDriver/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxGpioDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxGpioDriver_GTEST_BASE_HPP
#define LinuxGpioDriver_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_OpenError_SIZE(size) \
  this->assertEvents_GP_OpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_OpenError(index, _gpio, _error) \
  this->assertEvents_GP_OpenError(__FILE__, __LINE__, index, _gpio, _error)

#define ASSERT_EVENTS_GP_ConfigError_SIZE(size) \
  this->assertEvents_GP_ConfigError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_ConfigError(index, _gpio, _error) \
  this->assertEvents_GP_ConfigError(__FILE__, __LINE__, index, _gpio, _error)

#define ASSERT_EVENTS_GP_WriteError_SIZE(size) \
  this->assertEvents_GP_WriteError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_WriteError(index, _gpio, _error) \
  this->assertEvents_GP_WriteError(__FILE__, __LINE__, index, _gpio, _error)

#define ASSERT_EVENTS_GP_ReadError_SIZE(size) \
  this->assertEvents_GP_ReadError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_ReadError(index, _gpio, _error) \
  this->assertEvents_GP_ReadError(__FILE__, __LINE__, index, _gpio, _error)

#define ASSERT_EVENTS_GP_PortOpened_SIZE(size) \
  this->assertEvents_GP_PortOpened_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_PortOpened(index, _gpio) \
  this->assertEvents_GP_PortOpened(__FILE__, __LINE__, index, _gpio)

#define ASSERT_EVENTS_GP_IntStartError_SIZE(size) \
  this->assertEvents_GP_IntStartError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_IntStartError(index, _gpio) \
  this->assertEvents_GP_IntStartError(__FILE__, __LINE__, index, _gpio)

#define ASSERT_EVENTS_GP_IntWaitError_SIZE(size) \
  this->assertEvents_GP_IntWaitError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_GP_IntWaitError(index, _gpio) \
  this->assertEvents_GP_IntWaitError(__FILE__, __LINE__, index, _gpio)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_intOut_SIZE(size) \
  this->assert_from_intOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_intOut(index, _cycleStart) \
  { \
    ASSERT_GT(this->fromPortHistory_intOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_intOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_intOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_intOut& _e = \
      this->fromPortHistory_intOut->at(index); \
    ASSERT_EQ(_cycleStart, _e.cycleStart) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cycleStart at index " \
    << index \
    << " in history of from_intOut\n" \
    << "  Expected: " << _cycleStart << "\n" \
    << "  Actual:   " << _e.cycleStart << "\n"; \
  }

namespace Drv {

  //! \class LinuxGpioDriverGTestBase
  //! \brief Auto-generated base class for LinuxGpioDriver component Google Test harness
  //!
  class LinuxGpioDriverGTestBase :
    public LinuxGpioDriverTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxGpioDriverGTestBase
      //!
      LinuxGpioDriverGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxGpioDriverGTestBase
      //!
      virtual ~LinuxGpioDriverGTestBase(void);

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
      // Event: GP_OpenError
      // ----------------------------------------------------------------------

      void assertEvents_GP_OpenError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_OpenError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_ConfigError
      // ----------------------------------------------------------------------

      void assertEvents_GP_ConfigError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_ConfigError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_WriteError
      // ----------------------------------------------------------------------

      void assertEvents_GP_WriteError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_WriteError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_ReadError
      // ----------------------------------------------------------------------

      void assertEvents_GP_ReadError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_ReadError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio, /*!< The device*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_PortOpened
      // ----------------------------------------------------------------------

      void assertEvents_GP_PortOpened_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_PortOpened(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio /*!< The device*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_IntStartError
      // ----------------------------------------------------------------------

      void assertEvents_GP_IntStartError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_IntStartError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio /*!< The device*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_IntWaitError
      // ----------------------------------------------------------------------

      void assertEvents_GP_IntWaitError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_GP_IntWaitError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const I32 gpio /*!< The device*/
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
      // From port: intOut 
      // ----------------------------------------------------------------------

      void assert_from_intOut_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Drv

#endif
