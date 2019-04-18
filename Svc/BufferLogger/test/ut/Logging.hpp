// ======================================================================
// \title  Logging.hpp
// \author bocchino, mereweth
// \brief  Interface for BufferLogger logging tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
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
