module Svc {
    @ A sequence engine based around a WebAssembly interpreter
    active component WasmSequencer {
        include "WasmSequencerTypes.fppi"
        include "WasmSequencerCommands.fppi"
        include "WasmSequencerStateMachine.fppi"
        include "WasmSequencerParams.fppi"
        include "WasmSequencerEvents.fppi"
        include "WasmSequencerTelemetry.fppi"

        # All signals and async inputs are processed in order of emission; the
        # sequencer does not rely on queue priorities (see SDD).
        state machine instance sequencer: SequencerStateMachine assert

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

        @ Port for sending synchronous port invocation (no reply)
        output port serialSyncOut: [Svc.Fpy.SerialPortIndex.MAX_SERIAL_PORTS] serial

        @ Port for sending asynchronous port invocations (with reply on [serialAsyncReply])
        output port serialAsyncOut: [Svc.Fpy.SerialPortIndex.MAX_SERIAL_PORTS] serial

        @ Reply port for [serialAsyncOut]. This reply is subject to timeout if configured.
        @ Sequences that send serialAsyncOut messages will block until this reply is received
        @ on the corresponding port
        async input port serialAsyncReply: [Svc.Fpy.SerialPortIndex.MAX_SERIAL_PORTS] serial

        match serialAsyncOut with serialAsyncReply

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
