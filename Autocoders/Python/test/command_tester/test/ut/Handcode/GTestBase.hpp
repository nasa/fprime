// ======================================================================
// \title  CommandTester/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for CommandTester component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CommandTester_GTEST_BASE_HPP
#define CommandTester_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_cmdSendPort_SIZE(size) \
  this->assert_from_cmdSendPort_size(__FILE__, __LINE__, size)

#define ASSERT_from_cmdSendPort(index, _opCode, _cmdSeq, _args) \
  { \
    ASSERT_GT(this->fromPortHistory_cmdSendPort->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_cmdSendPort\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_cmdSendPort->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_cmdSendPort& _e = \
      this->fromPortHistory_cmdSendPort->at(index); \
    ASSERT_EQ(_opCode, _e.opCode) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument opCode at index " \
    << index \
    << " in history of from_cmdSendPort\n" \
    << "  Expected: " << _opCode << "\n" \
    << "  Actual:   " << _e.opCode << "\n"; \
    ASSERT_EQ(_cmdSeq, _e.cmdSeq) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cmdSeq at index " \
    << index \
    << " in history of from_cmdSendPort\n" \
    << "  Expected: " << _cmdSeq << "\n" \
    << "  Actual:   " << _e.cmdSeq << "\n"; \
    ASSERT_EQ(_args, _e.args) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument args at index " \
    << index \
    << " in history of from_cmdSendPort\n" \
    << "  Expected: " << _args << "\n" \
    << "  Actual:   " << _e.args << "\n"; \
  }

namespace Cmd {

  //! \class CommandTesterGTestBase
  //! \brief Auto-generated base class for CommandTester component Google Test harness
  //!
  class CommandTesterGTestBase :
    public CommandTesterTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CommandTesterGTestBase
      //!
      CommandTesterGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CommandTesterGTestBase
      //!
      virtual ~CommandTesterGTestBase(void);

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
      // From port: cmdSendPort
      // ----------------------------------------------------------------------

      void assert_from_cmdSendPort_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Cmd

#endif
