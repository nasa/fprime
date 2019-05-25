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


#include <Ref/MathReceiver/MathReceiverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  MathReceiverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    MathReceiverComponentImpl(
        const char *const compName
    ) :
      MathReceiverComponentBase(compName)
#else
    MathReceiverImpl(void)
#endif
    ,m_factor1(0.0)
    ,m_factor1s(0)
  {

  }

  void MathReceiverComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    MathReceiverComponentBase::init(queueDepth, instance);
  }

  MathReceiverComponentImpl ::
    ~MathReceiverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MathReceiverComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
      // declare result serializable
      Ref::MathOp op;
      F32 res = 0.0;
      switch (operation) {
          case MATH_ADD:
              op.setop(ADD);
              res = (val1 + val2)*this->m_factor1;
              break;
          case MATH_SUB:
              op.setop(SUB);
              res = (val1 - val2)*this->m_factor1;
              break;
          case MATH_MULTIPLY:
              op.setop(MULT);
              res = (val1 * val2)*this->m_factor1;
              break;
          case MATH_DIVIDE:
              op.setop(DIVIDE);
              res = (val1 / val2)*this->m_factor1;
              break;
          default:
              FW_ASSERT(0,operation);
              break;
      }
      Fw::ParamValid valid;
      res = res/this->paramGet_factor2(valid);

      op.setval1(val1);
      op.setval2(val2);
      op.setresult(res);
      this->log_ACTIVITY_HI_MR_OPERATION_PERFORMED(op);
      this->tlmWrite_MR_OPERATION(op);
      this->mathOut_out(0,res);
  }

  void MathReceiverComponentImpl ::
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

  void MathReceiverComponentImpl ::
    MR_SET_FACTOR1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val
    )
  {
      this->m_factor1 = val;
      this->log_ACTIVITY_HI_MR_SET_FACTOR1(val);
      this->tlmWrite_MR_FACTOR1(val);
      this->tlmWrite_MR_FACTOR1S(++this->m_factor1s);
      // reply with completion status
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MathReceiverComponentImpl ::
    MR_CLEAR_EVENT_THROTTLE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      // clear throttle
      this->log_ACTIVITY_HI_MR_SET_FACTOR1_ThrottleClear();
      // send event that throttle is cleared
      this->log_ACTIVITY_HI_MR_THROTTLE_CLEARED();
      // reply with completion status
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MathReceiverComponentImpl ::
     parameterUpdated(
      FwPrmIdType id /*!< The parameter ID*/
  ) {
      if (id == PARAMID_FACTOR2) {
        Fw::ParamValid valid;
        F32 val = this->paramGet_factor2(valid);
        this->log_ACTIVITY_HI_MR_UPDATED_FACTOR2(val);
      }
  }


} // end namespace Ref
