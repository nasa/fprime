# CCSDS TM/TC transfer frame layer: the TM framer (downlink), and the frame
# accumulator + TC deframer (uplink).
#
# Instance names are fully qualified so that this file may be included from
# topologies defined in other modules.

instance ComCcsds.framer
instance ComCcsds.tcDeframer
instance ComCcsds.frameAccumulator

connections Uplink {
    # FrameAccumulator <-> TcDeframer
    ComCcsds.frameAccumulator.dataOut -> ComCcsds.tcDeframer.dataIn
    ComCcsds.tcDeframer.dataReturnOut -> ComCcsds.frameAccumulator.dataReturnIn
}
