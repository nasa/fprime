module Svc {
    @ Dispatches command sequences to available command sequencers
    active component FpySequencer {

        enum BlockState {
            BLOCK
            NO_BLOCK
        }

        enum GoalState {
            RUNNING
            VALID
            IDLE
        }

        enum FileReadStage {
            HEADER
            BODY
            FOOTER
        }

        include "FpySequencerCommands.fppi"
        include "FpySequencerTelemetry.fppi"
        include "FpySequencerEvents.fppi"
        include "FpySequencerStateMachine.fppi"
        include "FpySequencerDirectives.fppi"
        include "FpySequencerParams.fppi"

        # sm signals have highest priority besides ping
        state machine instance sequencer: SequencerStateMachine priority 9 assert

        @ output port for commands from the seq
        output port cmdOut: Fw.Com

        @ responses back from commands from the seq
        # cmd responses have lower prio than sm sigs, cmds and ping
        async input port cmdResponseIn: Fw.CmdResponse priority 5 assert

        @ Ping in port
        # TODO should ping have highest prio? or lowest?
        async input port pingIn: Svc.Ping priority 10 assert

        @ port to trigger a wakeup or timeout check. increase frequency
        @ to increase temporal resolution of sequencer
        # timer check has lower prio than sm sigs, cmds, cmd resp and ping
        async input port checkTimers: Svc.Sched priority 4 assert

        @ port to write all telemetry
        # least important, lowest prio
        async input port tlmWrite: Svc.Sched priority 1 assert

        @ port for requests to run sequences
        # same priority as RUN cmd
        async input port seqRunIn: Svc.CmdSeqIn priority 7 assert

        @ called when a sequence begins running
        output port seqStartOut: Svc.CmdSeqIn

        @ called when a sequence finishes running, either successfully or not
        output port seqDoneOut: Fw.CmdResponse

        @ Ping out port
        output port pingOut: Svc.Ping

        @ port for getting telemetry channel values and storing them in sequence serRegs
        output port getTlmChan: Fw.TlmGet

        @ port for getting param values and storing them in sequence serRegs
        output port getParam: Fw.PrmGet

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        param get port prmGet

        param set port prmSet

    }
}