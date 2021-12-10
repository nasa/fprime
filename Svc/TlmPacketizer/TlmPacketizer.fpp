module Svc {

  @ A component for storing telemetry
  active component TlmPacketizer {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Packet send port
    output port PktSend: Fw.Com

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    @ Run port for starting packet send cycle
    async input port Run: Svc.Sched

    @ Telemetry input port
    sync input port TlmRecv: Fw.Tlm

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive
    command recv port cmdIn

    @ Command registration
    command reg port cmdRegOut

    @ Command response
    command resp port cmdResponseOut

    @ Event
    event port eventOut

    @ Telemetry
    telemetry port tlmOut

    @ Text event
    text event port textEventOut

    @ Time get
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Set telemetry send level
    async command SET_LEVEL(
                             level: U32 @< The I32 command argument
                           ) \
      opcode 0

    @ Force a packet to be sent
    async command SEND_PKT(
                            $id: U32 @< The packet ID
                          ) \
      opcode 1

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Telemetry channel is not part of a telemetry packet.
    event NoChan(
                  Id: U32 @< The telemetry ID
                ) \
      severity warning low \
      id 0 \
      format "Telemetry ID 0x{x} not packetized"

    @ Telemetry send level set
    event LevelSet(
                    $id: U32 @< The level
                  ) \
      severity activity high \
      id 1 \
      format "Telemetry send level to {}"

    @ Telemetry send level set
    event MaxLevelExceed(
                          level: U32 @< The level
                          max: U32 @< The max packet level
                        ) \
      severity warning low \
      id 2 \
      format "Requested send level {} higher than max packet level of {}"

    @ Packet manually sent
    event PacketSent(
                      $id: U32 @< The packet ID
                    ) \
      severity activity low \
      id 3 \
      format "Sent packet ID {}"

    @ Couldn't find the packet to send
    event PacketNotFound(
                          $id: U32 @< The packet ID
                        ) \
      severity warning low \
      id 4 \
      format "Could not find packet ID {}"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Telemetry send level
    telemetry SendLevel: U32 id 0

  }

}
