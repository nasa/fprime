module Svc {
    @ A sequence engine based around a WebAssembly interpreter
    active component WasmSequencer {

        include "WasmSequencerCommands.fppi"
        include "WasmSequencerStateMachine.fppi"

        # sm signals have highest priority besides ping
        state machine instance sequencer: SequencerStateMachine priority 9 assert

        @ Port for dispatching commands
        output port cmdOut: Fw.Com

        @ Response
        async input port cmdResponseIn: Fw.CmdResponse priority 5 assert

        @ Port to periodically drive sleep-wake and statement-timeout checks
        async input port checkTimers: Svc.Sched priority 4 assert

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