// ======================================================================
// \title  TlmChan/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TlmChan component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TlmChan_GTEST_BASE_HPP
#define TlmChan_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_PktSend_SIZE(size) \
  this->assert_from_PktSend_size(__FILE__, __LINE__, size)

#define ASSERT_from_PktSend(index, data, context) \
  { \
    ASSERT_GT(this->fromPortHistory_PktSend->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_PktSend\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_PktSend->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_PktSend& _e = \
      this->fromPortHistory_PktSend->at(index); \
    ASSERT_EQ(data, _e.data) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument data at index " \
    << index \
    << " in history of from_PktSend\n" \
    << "  Expected: " << data << "\n" \
    << "  Actual:   " << _e.data << "\n"; \
    ASSERT_EQ(context, _e.context) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument context at index " \
    << index \
    << " in history of from_PktSend\n" \
    << "  Expected: " << context << "\n" \
    << "  Actual:   " << _e.context << "\n"; \
  }

namespace Svc {

  //! \class TlmChanGTestBase
  //! \brief Auto-generated base class for TlmChan component Google Test harness
  //!
  class TlmChanGTestBase :
    public TlmChanTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TlmChanGTestBase
      //!
      TlmChanGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TlmChanGTestBase
      //!
      virtual ~TlmChanGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // From ports 
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: PktSend 
      // ----------------------------------------------------------------------

      void assert_from_PktSend_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

  };

} // end namespace Svc

#endif
