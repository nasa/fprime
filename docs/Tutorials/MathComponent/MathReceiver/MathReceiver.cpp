// ======================================================================
// \title  MathReceiver.cpp
// \author tcanham, bocchino
// \brief  cpp file for MathReceiver component implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include <FpConfig.hpp>
#include "Ref/MathReceiver/MathReceiver.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathReceiver ::
    MathReceiver(
        const char *const compName
    ) :
      MathReceiverComponentBase(compName)
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
    mathOpIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        const MathOp& op,
        F32 val2
    )
  {

      // Get the initial result
      F32 res = 0.0;
      switch (op.e) {
          case MathOp::ADD:
              res = val1 + val2;
              break;
          case MathOp::SUB:
              res = val1 - val2;
              break;
          case MathOp::MUL:
              res = val1 * val2;
              break;
          case MathOp::DIV:
              res = val1 / val2;
              break;
          default:
              FW_ASSERT(0, op.e);
              break;
      }

      // Get the factor value
      Fw::ParamValid valid;
      F32 factor = paramGet_FACTOR(valid);
      FW_ASSERT(
          valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
          valid.e
      );

      // Multiply result by factor
      res *= factor;

      // Emit telemetry and events
      this->log_ACTIVITY_HI_OPERATION_PERFORMED(op);
      this->tlmWrite_OPERATION(op);

      // Emit result
      this->mathResultOut_out(0, res);

  }

  void MathReceiver ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      U32 numMsgs = this->m_queue.getNumMsgs();
      for (U32 i = 0; i < numMsgs; ++i) {
          (void) this->doDispatch();
      }

  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MathReceiver ::
    CLEAR_EVENT_THROTTLE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      // clear throttle
      this->log_ACTIVITY_HI_FACTOR_UPDATED_ThrottleClear();
      // send event that throttle is cleared
      this->log_ACTIVITY_HI_THROTTLE_CLEARED();
      // reply with completion status
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void MathReceiver ::
     parameterUpdated(FwPrmIdType id)
  {
      switch (id) {
          case PARAMID_FACTOR: {
              Fw::ParamValid valid;
              F32 val = this->paramGet_FACTOR(valid);
              FW_ASSERT(
                  valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
                  valid.e
              );
              this->log_ACTIVITY_HI_FACTOR_UPDATED(val);
              break;
          }
          default:
              FW_ASSERT(0, id);
              break;
      }
  }

} // end namespace Ref
