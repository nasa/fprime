// ======================================================================
// \title  PingReceiverImpl.cpp
// \author tim
// \brief  cpp file for PingReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Ref/PingReceiver/PingReceiverComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PingReceiverComponentImpl ::
    PingReceiverComponentImpl(
        const char *const compName
    ) : PingReceiverComponentBase(compName), m_inhibitPings(false), m_pingsRecvd(0)
  {

  }

  void PingReceiverComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    PingReceiverComponentBase::init(queueDepth, instance);
  }

  PingReceiverComponentImpl ::
    ~PingReceiverComponentImpl()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void PingReceiverComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    //this->log_DIAGNOSTIC_PR_PingReceived(key);
    this->tlmWrite_PR_NumPings(this->m_pingsRecvd++);
    if (not this->m_inhibitPings) {
        PingOut_out(0,key);
    }
  }

  void PingReceiverComponentImpl::PR_StopPings_cmdHandler(
          FwOpcodeType opCode, /*!< The opcode*/
          U32 cmdSeq /*!< The command sequence number*/
      ) {
      this->m_inhibitPings = true;
      this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Ref
