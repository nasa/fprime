module Svc {
    @ A sequence engine based around a WebAssembly interpreter
    active component WasmSequencer {
        include "WasmSequencerTypes.fppi"
        include "WasmSequencerCommands.fppi"
        include "WasmSequencerControllerStateMachine.fppi"
        include "WasmSequencerInterpreterStateMachine.fppi"
        include "WasmSequencerParams.fppi"
        include "WasmSequencerEvents.fppi"
        include "WasmSequencerTelemetry.fppi"

        state machine instance controller: ControllerStateMachine assert
        state machine instance interpreter: InterpreterStateMachine assert

        @ Port for dispatching commands
        output port cmdOut: Fw.Com

        @ Command response input
        async input port cmdResponseIn: Fw.CmdResponse assert

        @ Port for getting telemetry channel values (backs the guest `tlm` host function)
        output port getTlmChan: Fw.TlmGet

        @ Port for getting parameter values (backs the guest `prm` host function)
        output port getParam: Fw.PrmGet

        @ Port to periodically drive sleep-wake and host-function-timeout checks
        async input port checkTimers: Svc.Sched assert

        @ Port to periodically write telemetry channels (optional)
        async input port writeTelemetry: Svc.Sched drop

        @ Port for sending a serial port invocation
        @ This is invoked by calling serial_out inside the wasm module
        @ If the port is not connected, the sequence will error out
        output port serialOut: [Wasm.MAX_SERIAL_OUT_PORTS] serial

        @ Port for receiving serial messages from other components.
        @ All port indices will be placed into their own internal binary queue to be
        @ handled by the serial_recv host function.
        guarded input port serialIn: [Wasm.MAX_SERIAL_IN_PORTS] serial

        @ port for requests to run sequences
        async input port seqRunIn: Svc.CmdSeqIn assert

        @ port for requesting to cancel the currently running sequence
        sync input port seqCancelIn: Svc.CmdSeqCancel

        @ called when a sequence begins running
        output port seqStartOut: Svc.CmdSeqIn

        @ called when a sequence finishes running, either successfully or not
        output port seqDoneOut: Fw.CmdResponse

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
