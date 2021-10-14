// ======================================================================
// \title  MathReceiverImpl.cpp
// \author tcanham
// \brief  cpp file for MathReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include "Fw/Types/BasicTypes.hpp"
#include "Ref/MathReceiver/MathReceiver.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathReceiver ::
    MathReceiver(
        const char *const compName
    ) : MathReceiverComponentBase(compName),
        m_factor1(0.0),
        m_factor1s(0)
  {

  }

  void MathReceiver ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    MathReceiverComponentBase::init(queueDepth, instance);
  }

  MathReceiver ::
    ~MathReceiver()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MathReceiver ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        MathOp op,
        F32 val2
    )
  {
      // declare result serializable
      F32 res = 0.0;
      switch (op.e) {
          case MathOp::ADD:
              res = (val1 + val2)*this->m_factor1;
              break;
          case MathOp::SUB:
              res = (val1 - val2)*this->m_factor1;
              break;
          case MathOp::MUL:
              res = (val1 * val2)*this->m_factor1;
              break;
          case MathOp::DIV:
              res = (val1 / val2)*this->m_factor1;
              break;
          default:
              FW_ASSERT(0, op.e);
              break;
      }
      Fw::ParamValid valid;
      res = res/this->paramGet_factor2(valid);

      this->log_ACTIVITY_HI_OPERATION_PERFORMED(op);
      this->tlmWrite_OPERATION(op);
      this->mathOut_out(0, res);
  }

  void MathReceiver ::
    SchedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      QueuedComponentBase::MsgDispatchStatus stat = QueuedComponentBase::MSG_DISPATCH_OK;
      // empty message queue
      while (stat != MSG_DISPATCH_EMPTY) {
          stat = this->doDispatch();
      }

  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MathReceiver ::
    SET_FACTOR1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val
    )
  {
      this->m_factor1 = val;
      this->log_ACTIVITY_HI_SET_FACTOR1(val);
      this->tlmWrite_FACTOR1(val);
      this->tlmWrite_FACTOR1S(++this->m_factor1s);
      // reply with completion status
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void MathReceiver ::
    CLEAR_EVENT_THROTTLE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      // clear throttle
      this->log_ACTIVITY_HI_SET_FACTOR1_ThrottleClear();
      // send event that throttle is cleared
      this->log_ACTIVITY_HI_THROTTLE_CLEARED();
      // reply with completion status
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void MathReceiver ::
     parameterUpdated(
      FwPrmIdType id /*!< The parameter ID*/
  ) {
      if (id == PARAMID_FACTOR2) {
        Fw::ParamValid valid;
        F32 val = this->paramGet_factor2(valid);
        this->log_ACTIVITY_HI_UPDATED_FACTOR2(val);
      }
  }


} // end namespace Ref
