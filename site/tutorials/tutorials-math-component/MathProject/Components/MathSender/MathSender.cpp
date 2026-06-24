// ======================================================================
// \title  MathSender.cpp
// \author asloan
// \brief  cpp file for MathSender component implementation class
// ======================================================================

#include <MathProject/Components/MathSender/MathSender.hpp>
#include <config/FpConfig.hpp>

namespace MathProject
{

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathSender ::
      MathSender(
          const char *const compName) : MathSenderComponentBase(compName)
  {
  }

  MathSender ::
      ~MathSender()
  {
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MathSender ::
      mathResultIn_handler(
          const FwIndexType portNum,
          F32 result)
  {
    this->tlmWrite_RESULT(result);
    this->log_ACTIVITY_HI_RESULT(result);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MathSender ::
      DO_MATH_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          F32 val1,
          MathProject::MathOp op,
          F32 val2)
  {
    this->tlmWrite_VAL1(val1);
    this->tlmWrite_OP(op);
    this->tlmWrite_VAL2(val2);
    this->log_ACTIVITY_LO_COMMAND_RECV(val1, op, val2);
    this->mathOpOut_out(0, val1, op, val2);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

} // end namespace MathProject
