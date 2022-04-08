// ======================================================================
// \title  epsPayloadPowerComponent.cpp
// \author nishq
// \brief  cpp file for epsPayloadPowerComponent component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Ref/adapter/eps/hardware/payloadPower/component/epsPayloadPowerComponent.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  epsPayloadPowerComponent ::
    epsPayloadPowerComponent(
        const char *const compName
    ) : epsPayloadPowerComponentComponentBase(compName)
  {

  }

  void epsPayloadPowerComponent ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    epsPayloadPowerComponentComponentBase::init(instance);
  }

  epsPayloadPowerComponent ::
    ~epsPayloadPowerComponent()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Ref::cmdRecv epsPayloadPowerComponent ::
    receivePowerCommandIn_handler(
        const NATIVE_INT_TYPE portNum,
        bool power
    )
  {
    // TODO return
  }

  Ref::cmdRecv epsPayloadPowerComponent ::
    receivePowerStatusIn_handler(
        const NATIVE_INT_TYPE portNum,
        bool status
    )
  {
    // TODO return
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void epsPayloadPowerComponent ::
    PAYLOAD_POWER_CMD_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        bool power
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Ref
