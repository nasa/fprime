// ======================================================================
// \title  SignalGen.cpp
// \author mstarch
// \brief  cpp file for SignalGen component implementation class
// ======================================================================


#include <Ref/SignalGen/SignalGen.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SignalGen ::
    SignalGen(
        const char *const compName
    ) : SignalGenComponentBase(compName)
  {

  }

  void SignalGen ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    SignalGenComponentBase::init(queueDepth, instance);
  }

  SignalGen ::
    ~SignalGen()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SignalGen ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void SignalGen ::
    SignalGen_Settings_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 Frequency,
        F32 Amplitude,
        F32 Phase,
        Ref::SignalType SigType
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void SignalGen ::
    SignalGen_Toggle_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void SignalGen ::
    SignalGen_Skip_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Ref
