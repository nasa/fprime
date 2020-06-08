// ======================================================================
// \title  ActiveTextLogger/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for ActiveTextLogger component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ======================================================================

#ifndef ActiveTextLogger_GTEST_BASE_HPP
#define ActiveTextLogger_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Svc {

  //! \class ActiveTextLoggerGTestBase
  //! \brief Auto-generated base class for ActiveTextLogger component Google Test harness
  //!
  class ActiveTextLoggerGTestBase :
    public ActiveTextLoggerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ActiveTextLoggerGTestBase
      //!
      ActiveTextLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ActiveTextLoggerGTestBase
      //!
      virtual ~ActiveTextLoggerGTestBase(void);

  };

} // end namespace Svc

#endif
