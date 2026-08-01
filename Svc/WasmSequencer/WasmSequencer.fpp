module Svc {
    @ A sequence engine based around a WebAssembly interpreter
    active component WasmSequencer {

        include "WasmSequencerTypes.fppi"
        include "WasmSequencerCommands.fppi"
        include "WasmSequencerStateMachine.fppi"
        include "WasmSequencerParams.fppi"
        include "WasmSequencerEvents.fppi"
        # include "WasmSequencerTelemetry.fppi"

        # All signals and async inputs are processed in order of emission; the
        # sequencer does not rely on queue priorities (see SDD).
        state machine instance sequencer: SequencerStateMachine assert

        @ Port for dispatching commands
        output port cmdOut: Fw.Com

        @ Response
        async input port cmdResponseIn: Fw.CmdResponse assert

        @ Port for getting telemetry channel values (backs the guest `tlm` host function)
        output port getTlmChan: Fw.TlmGet

        @ Port for getting parameter values (backs the guest `prm` host function)
        output port getParam: Fw.PrmGet

        @ Port to periodically drive sleep-wake and statement-timeout checks
        async input port checkTimers: Svc.Sched assert

        @ Port to periodically write telemetry channels (optional)
        async input port writeTelemetry: Svc.Sched drop

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