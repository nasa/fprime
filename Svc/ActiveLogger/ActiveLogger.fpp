module Svc {

  @ A component for logging events
  active component ActiveLogger {

    # ----------------------------------------------------------------------
    # Included files 
    # ----------------------------------------------------------------------

    include "ActiveLoggerCmdDict.fppi"

    include "ActiveLoggerEvrDict.fppi"

    include "ActiveLoggerIntIFDict.fppi"

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Event input port
    sync input port LogRecv: Fw.Log

    @ Packet send port
    output port PktSend: Fw.Com

    @ FATAL event announce port
    output port FatalAnnounce: Svc.FatalEvent

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Port for receiving commands
    command recv port CmdDisp

    @ Port for sending command registration requests
    command reg port CmdReg

    @ Port for sending command responses
    command resp port CmdStatus

    @ Port for emitting events
    event port Log

    @ Port for emitting text events
    text event port LogText

    @ Port for getting the time
    time get port Time
    
  }

}
