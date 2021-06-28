module Ref {

  @ A rate group active component with input and output scheduler ports
  queued component SendBuff {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ Active state
    enum ActiveState {
      SEND_IDLE
      SEND_ACTIVE
    }

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ The rate group scheduler input
    sync input port SchedIn: Svc.Sched

    @ The data buffer output
    output port Data: Drv.DataBuffer

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

    @ Param get port
    param get port ParamGet

    @ Param set port
    param set port ParamSet

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Command to start sending packets
    async command SB_START_PKTS \
      opcode 0

    @ Send a bad packet
    async command SB_INJECT_PKT_ERROR \
      opcode 1

    @ Generate a FATAL EVR
    async command SB_GEN_FATAL(
                                arg1: U32 @< First FATAL Argument
                                arg2: U32 @< Second FATAL Argument
                                arg3: U32 @< Third FATAL Argument
                              ) \
      opcode 2

    @ Generate an ASSERT
    async command SB_GEN_ASSERT(
                                 arg1: U32 @< First ASSERT Argument
                                 arg2: U32 @< Second ASSERT Argument
                                 arg3: U32 @< Third ASSERT Argument
                                 arg4: U32 @< Fourth ASSERT Argument
                                 arg5: U32 @< Fifth ASSERT Argument
                                 arg6: U32 @< Sixth ASSERT Argument
                               ) \
      opcode 3

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ First packet send
    event FirstPacketSent(
                           $id: U32 @< The ID argument
                         ) \
      severity activity high \
      id 0 \
      format "First packet ID {} received"

    @ Packet checksum error
    event PacketErrorInserted(
                               $id: U32 @< The ID argument
                             ) \
      severity warning high \
      id 1 \
      format "Inserted error in packet ID {}"

    @ Report parameter update
    event BuffSendParameterUpdated(
                                    $id: U32 @< The ID argument
                                  ) \
      severity activity low \
      id 2 \
      format "BuffSend Parameter {} was updated"

    @ A test FATAL
    event SendBuffFatal(
                         arg1: U32 @< First FATAL argument
                         arg2: U32 @< Second FATAL argument
                         arg3: U32 @< Second FATAL argument
                       ) \
      severity fatal \
      id 3 \
      format "Test Fatal: {} {} {}"

    # ----------------------------------------------------------------------
    # Parameters
    # ----------------------------------------------------------------------

    @ A test parameter
    param parameter3: U8 default 12 id 0 \
      set opcode 10 \
      save opcode 11

    @ A test parameter
    param parameter4: F32 default 13.14 id 1 \
      set opcode 12 \
      save opcode 13

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of packets sent
    telemetry PacketsSent: U64 id 0

    @ Number of errors injected
    telemetry NumErrorsInjected: U32 id 1 update on change

    @ Readback of Parameter3
    telemetry Parameter3: U8 id 2 update on change

    @ Readback of Parameter4
    telemetry Parameter4: F32 id 3 update on change

    @ Readback of Parameter4
    telemetry SendState: ActiveState id 4

  }

}
