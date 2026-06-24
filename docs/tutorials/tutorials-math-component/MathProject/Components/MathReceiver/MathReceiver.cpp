// ======================================================================
// \title  MathReceiver.cpp
// \author asloan
// \brief  cpp file for MathReceiver component implementation class
// ======================================================================

#include <MathProject/Components/MathReceiver/MathReceiver.hpp>
#include <config/FpConfig.hpp>

namespace MathProject
{

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathReceiver ::
      MathReceiver(
          const char *const compName) : MathReceiverComponentBase(compName),
                                        numMathOps(0)
  {
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
          const FwIndexType portNum,
          F32 val1,
          const MathProject::MathOp &op,
          F32 val2)
  {
    // Get the initial result
    F32 res = 0.0;
    switch (op.e)
    {
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
      if (val2 == 0)
      {
        this->log_ACTIVITY_HI_DIVIDE_BY_ZERO();
        break;
      }
      res = val1 / val2;
      break;
    default:
      FW_ASSERT(0, op.e);
      break;
    } // end switch

    // Get the factor value
    Fw::ParamValid valid;
    F32 factor = paramGet_FACTOR(valid);
    FW_ASSERT(
        valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
        valid.e);

    // Multiply result by factor
    res *= factor;

    // Increment number of math ops
    numMathOps++;

    // Emit telemetry and events
    this->log_ACTIVITY_HI_OPERATION_PERFORMED(op);
    this->tlmWrite_OPERATION(op);
    this->tlmWrite_NUMBER_OF_OPS(numMathOps);

    // Emit result
    this->mathResultOut_out(0, res);
  } // end mathOpIn_handler

  void MathReceiver ::
      schedIn_handler(
          const FwIndexType portNum,
          U32 context)
  {
    U32 numMsgs = this->m_queue.getMessagesAvailable();
    for (U32 i = 0; i < numMsgs; ++i)
    {
      (void)this->doDispatch();
    }
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MathReceiver ::
      CLEAR_EVENT_THROTTLE_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // clear throttle
    this->log_ACTIVITY_HI_FACTOR_UPDATED_ThrottleClear();
    // send event that throttle is cleared
    this->log_ACTIVITY_HI_THROTTLE_CLEARED();
    // reply with completion status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // Parameter Checker

  // In: MathReceiver.cpp
  void MathReceiver ::
      parameterUpdated(FwPrmIdType id)
  {
    switch (id)
    {
    case PARAMID_FACTOR:
    {
      Fw::ParamValid valid;
      F32 val = this->paramGet_FACTOR(valid);
      FW_ASSERT(
          valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
          valid.e);
      this->log_ACTIVITY_HI_FACTOR_UPDATED(val);
      break;
    }
    default:
      FW_ASSERT(0, id);
      break;
    }
  }

} // end namespace MathProject
