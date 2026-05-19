module Svc {
  @ A component for storing telemetry
  active component TlmChan {
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Events
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

      @Epoch Processing cap reached; one or more telemetry entries were deferred this cycle
    event TlmChanEpochProcessingCapReached(
        numDeferred: U32 @< Entries skipped (dropped) this invocation
        numTimesDeferredCountReached: U32 @< Cumulative invocations where cap was reached
    ) severity warning high \
    format "TlmChan epoch processing cap reached: {} entries deferred (cumulative: {})"

#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Ports
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

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

    @ A port for getting the time
    time get port timeCaller

    @ Event port for emitting events
    event port eventOut

    @ Event port for emitting text events
    text event port eventOutText
  }
}
