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

#include "BufferLoggerTester.hpp"

namespace Svc {

  namespace Logging {

    class BufferLoggerTester :
      public Svc::BufferLoggerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Test logging of data from bufferSendIn
        void BufferSendIn();

        //! Test close file command
        void CloseFile();

        //! Test logging of data from comIn
        void ComIn();

        //! Test logging on/off capability
        void OnOff();

    };

  }

}

#endif
