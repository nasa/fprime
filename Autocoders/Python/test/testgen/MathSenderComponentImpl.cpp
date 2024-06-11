// ======================================================================
// \title  MathSenderComponentImpl.cpp
// \author jishii
// \brief  cpp file for MathSender component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Autocoders/Python/test/testgen/MathSenderComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathSenderComponentImpl ::
    MathSenderComponentImpl(
        const char *const compName
    ) : MathSenderComponentBase(compName)
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
    ~MathSenderComponentImpl()
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
    MathOpTlm opTlm = ADD_TLM;
    MathOperation opPort = MATH_ADD;
    MathOpEv opEv = ADD_EV;

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
    }

    this->tlmWrite_MS_OP(opTlm);
    this->tlmWrite_MS_VAL1(val1);
    this->tlmWrite_MS_VAL2(val2);
    this->log_ACTIVITY_LO_MS_COMMAND_RECV(val1,val2,opEv);
    this->mathOut_out(0,val1,val2,opPort);
    // reply with completion status
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Ref
