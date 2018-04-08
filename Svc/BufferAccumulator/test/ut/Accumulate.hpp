// ======================================================================
// \title  Accumulate.hpp
// \author bocchino, mereweth
// \brief  Test accumulate mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
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

#ifndef Svc_Accumulate_HPP
#define Svc_Accumulate_HPP

#include "Tester.hpp"

namespace Svc {

  namespace Accumulate {

    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Send some buffers
        void OK(void);

    };

  }

}

#endif
