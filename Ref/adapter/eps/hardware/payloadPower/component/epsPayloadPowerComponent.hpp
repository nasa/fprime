// ======================================================================
// \title  epsPayloadPowerComponent.hpp
// \author nishq
// \brief  hpp file for epsPayloadPowerComponent component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef epsPayloadPowerComponent_HPP
#define epsPayloadPowerComponent_HPP

#include "Ref/adapter/eps/hardware/payloadPower/component/epsPayloadPowerComponentComponentAc.hpp"

namespace Ref {

  class epsPayloadPowerComponent :
    public epsPayloadPowerComponentComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object epsPayloadPowerComponent
      //!
      epsPayloadPowerComponent(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object epsPayloadPowerComponent
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object epsPayloadPowerComponent
      //!
      ~epsPayloadPowerComponent();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for receivePowerCommandIn
      //!
      Ref::cmdRecv receivePowerCommandIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool power /*!< 
      boolean val, true -> ON, false -> OFF
      */
      );

      //! Handler implementation for receivePowerStatusIn
      //!
      Ref::cmdRecv receivePowerStatusIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool status /*!< 
      boolean val, true -> ON, false -> OFF
      */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for PAYLOAD_POWER_CMD command handler
      //! Command that receives a power signal for payload from the ground station
      void PAYLOAD_POWER_CMD_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          bool power /*!< 
          boolean val, true -> ON, false -> OFF
          */
      );


    };

} // end namespace Ref

#endif
