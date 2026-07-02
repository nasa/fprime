module Svc {
    @ A sequence engine based around a WebAssembly interpreter
    active component WasmSequencer {

        include "WasmSequencerCommands.fppi"

        @ Port for dispatching commands
        output port cmdOut: Fw.Com

        @ Response 
        async input port cmdResponseIn: Fw.CmdResponse priority 5 assert

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