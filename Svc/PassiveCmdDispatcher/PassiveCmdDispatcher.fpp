module Svc {

    @ A passive component for dispatching commands
    passive component PassiveCmdDispatcher {

        ###############################################################################
        # Commands
        ###############################################################################

        @ No-op command
        sync command CMD_NO_OP \
            opcode 0x0

        @ Clear command tracking info to recover from components that are not returning status
        sync command CMD_CLEAR_TRACKING \
            opcode 0x1

        ###############################################################################
        # Events
        ###############################################################################

        @ Command dispatched
        event OpCodeDispatched($opcode: FwOpcodeType, $port: I32) \
            severity command \
            id 0x0 \
            format "Opcode 0x{x} dispatched to port {}"

        @ Command success event
        event OpCodeCompleted($opcode: FwOpcodeType) \
            severity command \
            id 0x1 \
            format "Opcode 0x{x} completed"

        @ Command failure event
        event OpCodeError($opcode: FwOpcodeType, error: Fw.CmdResponse) \
            severity command \
            id 0x2 \
            format "Opcode 0x{x} completed with error {}"

        @ Received a malformed command packet
        event MalformedCommand($status: Fw.DeserialStatus) \
            severity warning high \
            id 0x3 \
            format "Received malformed command packet. Status: {}"

        @ Received an invalid opcode
        event InvalidCommand($opcode: FwOpcodeType) \
            severity warning high \
            id 0x4 \
            format "Invalid opcode 0x{x} received"

        @ Exceeded the number of commands that can be executed simultaneously
        event TooManyCommands($opcode: FwOpcodeType) \
            severity warning high \
            id 0x5 \
            format "Too many outstanding commands. Opcode: 0x{x}"

        @ Received a NO_OP command
        event NoOpReceived \
            severity activity high \
            id 0x6 \
            format "Received a NO_OP command"

        ###############################################################################
        # General Ports
        ###############################################################################

        @ Command registration input ports. Size must match the dispatch output ports.
        sync input port compCmdReg: [CmdDispatcherComponentCommandPorts] Fw.CmdReg

        @ Command dispatch output ports. Size must match the registration input ports.
        output port compCmdSend: [CmdDispatcherComponentCommandPorts] Fw.Cmd

        @ Input command status ports
        sync input port compCmdStat: Fw.CmdResponse

        @ Command buffer input port for sequencers or other sources of command buffers
        sync input port seqCmdBuff: [CmdDispatcherSequencePorts] Fw.Com

        @ Output command sequence status ports
        output port seqCmdStatus: [CmdDispatcherSequencePorts] Fw.CmdResponse

        # Port matching specifiers
        match compCmdSend with compCmdReg
        match seqCmdStatus with seqCmdBuff

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters
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
    }
}