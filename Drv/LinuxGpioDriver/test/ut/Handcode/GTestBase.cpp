// ======================================================================
// \title  LinuxGpioDriver/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for LinuxGpioDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LinuxGpioDriverGTestBase ::
    LinuxGpioDriverGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        LinuxGpioDriverTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  LinuxGpioDriverGTestBase ::
    ~LinuxGpioDriverGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_OpenError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_OpenError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_OpenError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_OpenError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_OpenError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_OpenError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_OpenError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_OpenError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_OpenError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_OpenError& e =
      this->eventHistory_GP_OpenError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_OpenError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event GP_OpenError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_ConfigError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_ConfigError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_ConfigError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_ConfigError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_ConfigError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_ConfigError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_ConfigError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_ConfigError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_ConfigError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_ConfigError& e =
      this->eventHistory_GP_ConfigError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_ConfigError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event GP_ConfigError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_WriteError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_WriteError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_WriteError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_WriteError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_WriteError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_WriteError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_WriteError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_WriteError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_WriteError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_WriteError& e =
      this->eventHistory_GP_WriteError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_WriteError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event GP_WriteError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_ReadError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_ReadError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_ReadError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_ReadError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_ReadError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_ReadError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_ReadError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_ReadError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_ReadError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_ReadError& e =
      this->eventHistory_GP_ReadError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_ReadError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event GP_ReadError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_PortOpened
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_PortOpened_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_PortOpened->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_PortOpened\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_PortOpened->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_PortOpened(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_PortOpened->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_PortOpened\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_PortOpened->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_PortOpened& e =
      this->eventHistory_GP_PortOpened->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_PortOpened\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_IntStartError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_IntStartError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_IntStartError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_IntStartError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_IntStartError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_IntStartError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_IntStartError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_IntStartError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_IntStartError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_IntStartError& e =
      this->eventHistory_GP_IntStartError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_IntStartError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: GP_IntWaitError
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_IntWaitError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_GP_IntWaitError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event GP_IntWaitError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_GP_IntWaitError->size() << "\n";
  }

  void LinuxGpioDriverGTestBase ::
    assertEvents_GP_IntWaitError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 gpio
    ) const
  {
    ASSERT_GT(this->eventHistory_GP_IntWaitError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event GP_IntWaitError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_GP_IntWaitError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_GP_IntWaitError& e =
      this->eventHistory_GP_IntWaitError->at(index);
    ASSERT_EQ(gpio, e.gpio)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument gpio at index "
      << index
      << " in history of event GP_IntWaitError\n"
      << "  Expected: " << gpio << "\n"
      << "  Actual:   " << e.gpio << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assertFromPortHistory_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: intOut
  // ----------------------------------------------------------------------

  void LinuxGpioDriverGTestBase ::
    assert_from_intOut_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_intOut->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_intOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_intOut->size() << "\n";
  }

} // end namespace Drv
