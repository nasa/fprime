module Svc {
  @ A sequence engine based around a WebAssembly interpreter
  active component WasmSequencer {
    include "WasmSequencerTypes.fppi"
    include "WasmSequencerCommands.fppi"
    include "WasmSequencerControllerStateMachine.fppi"
    include "WasmSequencerEngineStateMachine.fppi"
    include "WasmSequencerParams.fppi"
    include "WasmSequencerEvents.fppi"
    include "WasmSequencerTelemetry.fppi"

    state machine instance controller: ControllerStateMachine assert
    state machine instance interpreter: EngineStateMachine assert

    @ Port for dispatching commands
    output port cmdOut: Fw.Com

    @ Command response input
    async input port cmdResponseIn: Fw.CmdResponse assert

    @ Port for getting telemetry channel values (backs the guest `tlm` host function)
    output port getTlmChan: Fw.TlmGet

    @ Port for getting parameter values (backs the guest `prm` host function)
    output port getParam: Fw.PrmGet

    @ Port to periodically drive sleep-wake and statement-timeout checks
    async input port checkTimers: Svc.Sched assert

    @ Port to periodically write telemetry channels (optional)
    async input port writeTelemetry: Svc.Sched drop

    @ Port for sending a serial port invocation
    @ The Wasm modules can call sync/async variants of this.
    @ If sync variant is called -> serialReply[portNum] MUST NOT be connected.
    @ If async variant is called -> serialReply[portNum] MUST be connected.
    output port serialOut: [Fpy.SerialPortIndex.MAX_SERIAL_PORTS] serial

    @ Reply port for [serialOut]. This reply is subject to timeout if configured.
    @ Sequences that send async serial messages will block until this reply is received
    @ on the corresponding port number
    async input port serialReply: [Fpy.SerialPortIndex.MAX_SERIAL_PORTS] serial

    @ port for requests to run sequences
    # same priority as RUN cmd
    async input port seqRunIn: Svc.CmdSeqIn assert

    @ port for requesting to cancel the currently running sequence
    # same priority as CANCEL cmd
    async input port seqCancelIn: Svc.CmdSeqCancel assert

    @ called when a sequence begins running
    output port seqStartOut: Svc.CmdSeqIn

    @ called when a sequence finishes running, either successfully or not
    output port seqDoneOut: Fw.CmdResponse

    match serialOut with serialReply

    ###############################################################################
    # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
    ###############################################################################
    @ Port for requesting the current time
    time get port timeCaller

    @ Enables command handling
    import Fw.Command

    @ Enables event handling
    import Fw.Event

    @ Enables telemetry channels handling
    import Fw.Channel

    @ Port to return the value of a parameter
    param get port prmGet

    @Port to set the value of a parameter
    param set port prmSet
  }
}
