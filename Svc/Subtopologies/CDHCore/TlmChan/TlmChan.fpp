module Svc {

  @ A component for storing telemetry
  active component TlmChan {

    @ Guarded port for receiving telemetry values
    guarded input port TlmRecv: Fw.Tlm

    @ Guarded port for returning telemetry values by reference
    guarded input port TlmGet: Fw.TlmGet

    @ Run port for starting packet send cycle
    async input port Run: Svc.Sched

    @ Packet send port
    output port PktSend: Fw.Com

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

  }

}
