module Svc {

  @ A component for storing telemetry
  active component ActiveLogger {

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerCmdDict.xml
    include "ActiveLoggerCmdDict.fpp"

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerEvrDict.xml
    include "ActiveLoggerEvrDict.fpp"

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerIntIFDict.xml
    include "ActiveLoggerIntIFDict.fpp"

    @ Telemetry input port
    sync input port LogRecv: Fw.Log

    @ Packet send port
    output port PktSend: Fw.Com

    @ FATAL event announce port
    output port FatalAnnounce: Svc.FatalEvent

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

    @ Port for receiving commands
    command recv port CmdIn

    @ Port for sending command registration requests
    command reg port cmdRegOut

    @ Port for sending command responses
    command resp port cmdResponseOut

    @ Port for emitting events
    event port eventOut

    @ Port for emitting text events
    text event port textEventOut

    @ Port for getting the time
    time get port timeGetOut

  }

}
