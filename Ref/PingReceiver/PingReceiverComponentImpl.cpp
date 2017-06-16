// ====================================================================== 
// \title  PingReceiverImpl.cpp
// \author tim
// \brief  cpp file for PingReceiver component implementation class
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


#include <Ref/PingReceiver/PingReceiverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  PingReceiverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    PingReceiverComponentImpl(
        const char *const compName
    ) :
      PingReceiverComponentBase(compName)
#else
    PingReceiverImpl(void)
#endif
    ,m_inhibitPings(false),m_pingsRecvd(0)
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
    ~PingReceiverComponentImpl(void)
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
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace Ref
