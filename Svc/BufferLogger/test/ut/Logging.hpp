// ======================================================================
// \title  Logging.hpp
// \author bocchino, mereweth
// \brief  Interface for BufferLogger logging tests
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

#ifndef Svc_Logging_HPP
#define Svc_Logging_HPP

#include "Tester.hpp"

namespace Svc {

  namespace Logging {

    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Test logging of data from bufferSendIn
        void BufferSendIn(void);

        //! Test close file command
        void CloseFile(void);

        //! Test logging of data from comIn
        void ComIn(void);

        //! Test logging on/off capability
        void OnOff(void);

    };

  }

}

#endif
