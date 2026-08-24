        #Active Components
        instance cmdDisp
        instance events
        instance tlmSend

        #Queued Components
        instance $health

        #Passive Components
        instance version
        instance fatalAdapter
        instance fatalHandler

        connections FaultProtection {
            events.FatalAnnounce -> fatalHandler.FatalReceive
        }

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        @ Input port for receiving command buffers from sequencers or other command buffer sources
        port seqCmdBuff   = cmdDisp.seqCmdBuff

        @ Output port returning command execution status to the command source
        port seqCmdStatus = cmdDisp.seqCmdStatus

        @ Output port for sending event packets from the EventManager
        port eventsPktSend  = events.PktSend

        @ Output port sending packetized telemetry to the comm stack for downlink
        port tlmSendPktSend = tlmSend.PktSend

        @ Input port to trigger a telemetry packet send cycle
        port tlmSendRun = tlmSend.Run

        @ Input port for scheduling the command dispatcher
        port cmdDispRun = cmdDisp.run

        @ Input port for scheduling the Health component
        port healthRun  = $health.Run

        @ Input port for scheduling the EventManager (dropped event telemetry)
        port eventsRun  = events.run
