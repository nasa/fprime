// ====================================================================== 
// \title  MathSenderImpl.cpp
// \author tcanham
// \brief  cpp file for MathSender component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 


#include <Ref/MathSender/MathSenderComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  MathSenderComponentImpl ::
#if FW_OBJECT_NAMES == 1
    MathSenderComponentImpl(
        const char *const compName
    ) :
      MathSenderComponentBase(compName)
#else
    MathSenderImpl(void)
#endif
  {

  }

  void MathSenderComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    MathSenderComponentBase::init(queueDepth, instance);
  }

  MathSenderComponentImpl ::
    ~MathSenderComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MathSenderComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
      this->tlmWrite_MS_RES(result);
      this->log_ACTIVITY_HI_MS_RESULT(result);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void MathSenderComponentImpl ::
    MS_DO_MATH_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val1,
        F32 val2,
        MathOp operation
    )
  {
    MathOpTlm opTlm;
    MathOperation opPort;
    MathOpEv opEv;
    switch (operation) {
      case ADD:
          opTlm = ADD_TLM;
          opPort = MATH_ADD;
          opEv = ADD_EV;
          break;
      case SUBTRACT:
          opTlm = SUB_TLM;
          opPort = MATH_SUB;
          opEv = SUB_EV;
          break;
      case MULTIPLY:
          opTlm = MULT_TLM;
          opPort = MATH_MULTIPLY;
          opEv = MULT_EV;
          break;
      case DIVIDE:
          opTlm = DIV_TLM;
          opPort = MATH_DIVIDE;
          opEv = DIV_EV;
          break;
      default:
          FW_ASSERT(0,operation);
          break;
    }

    this->tlmWrite_MS_OP(opTlm);
    this->tlmWrite_MS_VAL1(val1);
    this->tlmWrite_MS_VAL2(val2);
    this->log_ACTIVITY_LO_MS_COMMAND_RECV(val1,val2,opEv);
    this->mathOut_out(0,val1,val2,opPort);
    // reply with completion status
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace Ref
