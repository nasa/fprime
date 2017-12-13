// ====================================================================== 
// \title  Errors.hpp
// \author bocchino
// \brief  Interface for BufferLogger error tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
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

#ifndef ASTERIA_Errors_HPP
#define ASTERIA_Errors_HPP

#include "Tester.hpp"

namespace ASTERIA {

  namespace Errors {

    class Tester :
      public ASTERIA::Tester
    {

      public:

        // ---------------------------------------------------------------------- 
        // Tests
        // ---------------------------------------------------------------------- 

        //! Log file open error
        void LogFileOpen(void);

        //! Log file write error
        void LogFileWrite(void);

        //! Log file validation error
        void LogFileValidation(void);

        //! Non-volatile read error
        void NonVolatileRead(void);

        //! Non-volatile write error
        void NonVolatileWrite(void);

    };

  }

}

#endif
