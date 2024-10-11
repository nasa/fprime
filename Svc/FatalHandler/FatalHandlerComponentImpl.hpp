// ======================================================================
// \title  FatalHandlerImpl.hpp
// \author tcanham
// \brief  hpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FatalHandler_HPP
#define FatalHandler_HPP

#include "Svc/FatalHandler/FatalHandlerComponentAc.hpp"

namespace Svc {

  class FatalHandlerComponentImpl :
    public FatalHandlerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FatalHandler
      //!
      FatalHandlerComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object FatalHandler
      //!
      ~FatalHandlerComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for FatalReceive
      //!
      void FatalReceive_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType Id /*!< The ID of the FATAL event*/
      );


    };

} // end namespace Svc

#endif
