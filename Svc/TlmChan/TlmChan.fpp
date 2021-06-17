module Svc {

  @ A component for storing telemetry
  active component TlmChan {

    @ Telemetry input port
    guarded input port TlmRecv: Fw.Tlm

    @ Telemetry input port
    guarded input port TlmGet: Fw.Tlm

    @ Run port for starting packet send cycle
    async input port Run: Svc.Sched

    @ Packet send port
    output port PktSend: Fw.Com

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

  }

}
