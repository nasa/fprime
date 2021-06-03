module Svc {

  @ A component for storing telemetry
  active component ActiveLogger {


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

  }

}
