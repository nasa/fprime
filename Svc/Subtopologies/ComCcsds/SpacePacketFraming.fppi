# Space Packet packet layer: router, ComQueue, space packet framer/deframer,
# APID manager, aggregator, and comms buffer manager.
#
# Instance names are fully qualified so that this file may be included from
# topologies defined in other modules.

# Active Components
instance ComCcsds.comQueue

# Passive Components
instance ComCcsds.commsBufferManager
instance ComCcsds.fprimeRouter
instance ComCcsds.spacePacketDeframer
instance ComCcsds.spacePacketFramer
instance ComCcsds.apidManager
instance ComCcsds.aggregator

connections Downlink {
    # ComQueue <-> SpacePacketFramer
    ComCcsds.comQueue.dataOut                -> ComCcsds.spacePacketFramer.dataIn
    ComCcsds.spacePacketFramer.dataReturnOut -> ComCcsds.comQueue.dataReturnIn
    # SpacePacketFramer buffer and APID management
    ComCcsds.spacePacketFramer.bufferAllocate   -> ComCcsds.commsBufferManager.bufferGetCallee
    ComCcsds.spacePacketFramer.bufferDeallocate -> ComCcsds.commsBufferManager.bufferSendIn
    ComCcsds.spacePacketFramer.getApidSeqCount  -> ComCcsds.apidManager.getApidSeqCountIn
    # SpacePacketFramer <-> ComAggregator
    ComCcsds.spacePacketFramer.dataOut -> ComCcsds.aggregator.dataIn
    ComCcsds.aggregator.dataReturnOut  -> ComCcsds.spacePacketFramer.dataReturnIn

    # ComStatus
    ComCcsds.aggregator.comStatusOut        -> ComCcsds.spacePacketFramer.comStatusIn
    ComCcsds.spacePacketFramer.comStatusOut -> ComCcsds.comQueue.comStatusIn
    # (Outgoing) Aggregator <-> downstream connections shall be established by the user
}

connections Uplink {
    # (Incoming) downstream <-> SpacePacketDeframer connections shall be established by the user
    # SpacePacketDeframer APID validation
    ComCcsds.spacePacketDeframer.validateApidSeqCount -> ComCcsds.apidManager.validateApidSeqCountIn
    # SpacePacketDeframer <-> Router
    ComCcsds.spacePacketDeframer.dataOut -> ComCcsds.fprimeRouter.dataIn
    ComCcsds.fprimeRouter.dataReturnOut  -> ComCcsds.spacePacketDeframer.dataReturnIn
}
