module Ref {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
    rateGroup3
  }



  topology Ref {
    # ----------------------------------------------------------------------
    # Subtopology imports
    # ----------------------------------------------------------------------
    import CdhCore.Subtopology
    import ComCcsds.Subtopology
    import FileHandling.Subtopology
    import DataProducts.Subtopology

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance SG1
    instance SG2
    instance SG3
    instance SG4
    instance SG5
    instance blockDrv
    instance posixTime
    instance pingRcvr
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance recvBuffComp
    instance sendBuffComp
    instance typeDemo
    instance systemResources
    instance dpDemo
    instance linuxTimer
    instance comDriver
    instance cmdSeq

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance CdhCore.cmdDisp

    event connections instance CdhCore.events

    telemetry connections instance CdhCore.tlmSend

    text event connections instance CdhCore.textLogger

    health connections instance CdhCore.$health

    param connections instance FileHandling.prmDb

    time connections instance posixTime

    # ----------------------------------------------------------------------
    # Telemetry packets
    # ----------------------------------------------------------------------

    include "RefPackets.fppi"

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {

      # Linux timer to drive cycle
      linuxTimer.CycleOut -> rateGroupDriverComp.CycleIn

      # Rate group 1
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut[0] -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> CdhCore.tlmSendRun
      rateGroup1Comp.RateGroupMemberOut[3] -> FileHandling.fileDownlinkRun
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[5] -> ComCcsds.comQueueRun
      rateGroup1Comp.RateGroupMemberOut[6] -> CdhCore.cmdDispRun
      rateGroup1Comp.RateGroupMemberOut[7] -> ComCcsds.aggregatorTimeout

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn
      rateGroup2Comp.RateGroupMemberOut[4] -> dpDemo.run
      #connection to FileManager listing feature command for sequencing
      rateGroup2Comp.RateGroupMemberOut[5] -> FileHandling.fileManagerSchedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> CdhCore.healthRun
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> ComCcsds.bufferManagerSchedIn
      rateGroup3Comp.RateGroupMemberOut[4] -> DataProducts.dpBufferManagerSchedIn
      rateGroup3Comp.RateGroupMemberOut[5] -> DataProducts.dpWriterSchedIn
      rateGroup3Comp.RateGroupMemberOut[6] -> DataProducts.dpMgrSchedIn
    }

    connections Communications {
      # ComDriver buffer allocations
      comDriver.allocate   -> ComCcsds.commsBufferGetCallee
      comDriver.deallocate -> ComCcsds.commsBufferSendIn

      # ComDriver <-> ComStub (Uplink)
      comDriver.$recv              -> ComCcsds.drvReceiveIn
      ComCcsds.drvReceiveReturnOut -> comDriver.recvReturnIn

      # ComStub <-> ComDriver (Downlink)
      ComCcsds.drvSendOut -> comDriver.$send
      comDriver.ready     -> ComCcsds.drvConnected
    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data

      ### Moved this out of DataProducts Subtopology --> anything specific to deployment should live in Ref connections
      # Synchronous request. Will have both request kinds for demo purposes, not typical
      SG1.productGetOut -> DataProducts.productGetIn
      # Asynchronous request
      SG1.productRequestOut -> DataProducts.productRequestIn
      DataProducts.productResponseOut -> SG1.productRecvIn
      # Send filled DP
      SG1.productSendOut -> DataProducts.productSendIn
      # Synchronous request
      dpDemo.productGetOut -> DataProducts.productGetIn
      # Send filled DP
      dpDemo.productSendOut -> DataProducts.productSendIn
      # Asynchronous request
      dpDemo.productRequestOut -> DataProducts.productRequestIn
      DataProducts.productResponseOut -> dpDemo.productRecvIn
    }

    connections ComCcsds_CdhCore {
      # Events and telemetry to comQueue
      CdhCore.eventsPktSend  -> ComCcsds.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.EVENTS]
      CdhCore.tlmSendPktSend -> ComCcsds.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.TELEMETRY]

      # Router <-> CmdDispatcher
      ComCcsds.commandOut    -> CdhCore.seqCmdBuff
      CdhCore.seqCmdStatus   -> ComCcsds.cmdResponseIn
      cmdSeq.comCmdOut       -> CdhCore.seqCmdBuff
      CdhCore.seqCmdStatus   -> cmdSeq.cmdResponseIn
    }

    connections ComCcsds_FileHandling {
      # File Downlink <-> ComQueue
      FileHandling.fileDownlinkBufferSendOut -> ComCcsds.bufferQueueIn[ComCcsds.Ports_ComBufferQueue.FILE]
      ComCcsds.bufferReturnOut[ComCcsds.Ports_ComBufferQueue.FILE] -> FileHandling.fileDownlinkBufferReturn

      # Router <-> FileUplink
      ComCcsds.fileOut                     -> FileHandling.fileUplinkBufferSendIn
      FileHandling.fileUplinkBufferSendOut -> ComCcsds.fileBufferReturnIn
    }

    connections FileHandling_DataProducts {
      DataProducts.dpCatFileOut              -> FileHandling.fileDownlinkSendFile
      FileHandling.fileDownlinkFileComplete  -> DataProducts.dpCatFileDone
    }

  }

}
