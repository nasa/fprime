module Ref {

  @ Packet receive status
  enum PacketRecvStatus {
    PACKET_STATE_NO_PACKETS = 0
    PACKET_STATE_OK = 1
    PACKET_STATE_ERRORS = 3 @< Receiver has seen errors
  }

  @ Some Packet Statistics
  struct PacketStat {
    BuffRecv: U32 @< Number of buffers received
    BuffErr: U32 @< Number of buffers received with errors
    PacketStatus: PacketRecvStatus @< Packet Status
  }

  @ A rate group active component with input and output scheduler ports
  passive component RecvBuff {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ The data buffer input
    sync input port Data: Drv.DataBuffer

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

    @ A port for getting parameter values
    param get port ParamGet

    @ A port for setting parameter values
    param set port ParamSet

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ First packet received
    event FirstPacketReceived(
                               $id: U32 @< The ID argument
                             ) \
      severity activity low \
      id 0 \
      format "First packet ID {} received"

    @ Packet checksum error
    event PacketChecksumError(
                               $id: U32 @< The ID argument
                             ) \
      severity warning high \
      id 1 \
      format "Packet ID {} had checksum error"

    @ Report parameter update
    event BuffRecvParameterUpdated(
                                    $id: U32 @< The ID argument
                                  ) \
      severity activity low \
      id 2 \
      format "BuffRecv Parameter {} was updated"

    # ----------------------------------------------------------------------
    # Parameters
    # ----------------------------------------------------------------------

    @ A test parameter
    param parameter1: U32 default 10 id 0 \
      set opcode 0 \
      save opcode 1

    @ A test parameter
    param parameter2: I16 default 11 id 1 \
      set opcode 2 \
      save opcode 3

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Packet Statistics
    telemetry PktState: Ref.PacketStat id 0

    @ Value of Sensor1
    telemetry Sensor1: F32 id 1 \
    format "{.2f}V"

    @ Value of Sensor3
    telemetry Sensor2: F32 id 2

    @ Readback of Parameter1
    telemetry Parameter1: U32 id 3 update on change

    @ Readback of Parameter2
    telemetry Parameter2: I16 id 4 update on change \
      low {
        red -3
        orange -2
        yellow -1
      } \
      high {
        red 3
        orange 2
        yellow 1
      }

  }

}
