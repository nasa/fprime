module Svc {

  @ A component for storing telemetry
  active component TlmChan {

    @ Mutexed telemetry input port
    guarded input port TlmRecv: Fw.Tlm

    @ Mutexed telemetry input port
    guarded input port TlmGet: Fw.Tlm

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
