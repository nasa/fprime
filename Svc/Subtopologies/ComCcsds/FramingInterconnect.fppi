# Connections composing the SpacePacketFraming packet layer with the
# TmTcFraming TM/TC transfer frame layer to form the full CCSDS
# communications stack.
#
# Instance names are fully qualified so that this file may be included from
# topologies defined in other modules.

connections Downlink {
    # SpacePacketFraming <-> TmTcFraming
    ComCcsds.aggregator.dataOut   -> ComCcsds.framer.dataIn
    ComCcsds.framer.dataReturnOut -> ComCcsds.aggregator.dataReturnIn

    # ComStatus
    ComCcsds.framer.comStatusOut -> ComCcsds.aggregator.comStatusIn
    # (Outgoing) TmTcFraming <-> ComInterface connections shall be established by the user
}

connections Uplink {
    # (Incoming) ComInterface <-> TmTcFraming connections shall be established by the user
    # TmTcFraming buffer allocations
    ComCcsds.frameAccumulator.bufferDeallocate -> ComCcsds.commsBufferManager.bufferSendIn
    ComCcsds.frameAccumulator.bufferAllocate   -> ComCcsds.commsBufferManager.bufferGetCallee
    # TmTcFraming <-> SpacePacketFraming
    ComCcsds.tcDeframer.dataOut                -> ComCcsds.spacePacketDeframer.dataIn
    ComCcsds.spacePacketDeframer.dataReturnOut -> ComCcsds.tcDeframer.dataReturnIn
}
