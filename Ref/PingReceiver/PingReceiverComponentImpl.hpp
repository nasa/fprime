// ======================================================================
// \title  PingReceiverImpl.hpp
// \author tim
// \brief  hpp file for PingReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef PingReceiver_HPP
#define PingReceiver_HPP

#include "Ref/PingReceiver/PingReceiverComponentAc.hpp"

namespace Ref {

  class PingReceiverComponentImpl :
    public PingReceiverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object PingReceiver
      //!
      PingReceiverComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object PingReceiver
      //!
      ~PingReceiverComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      void PR_StopPings_cmdHandler(
              FwOpcodeType opCode, /*!< The opcode*/
              U32 cmdSeq /*!< The command sequence number*/
          );

      bool m_inhibitPings;
      U32 m_pingsRecvd;


    };

} // end namespace Ref

#endif
