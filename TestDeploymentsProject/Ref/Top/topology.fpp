module Ref {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
    rateGroup3
  }



  deployment topology Ref {
    # ----------------------------------------------------------------------
    # Subtopology instances
    # ----------------------------------------------------------------------
    instance CdhCore.Subtopology
    instance ComCcsdsSdls.Subtopology
    instance ComCcsdsSdls.keyManager
    instance FileHandling.Subtopology
    instance DataProducts.Subtopology
    #instance DpCompression.Subtopology

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
      rateGroup1Comp.RateGroupMemberOut[2] -> CdhCore.Subtopology.tlmSendRun
      rateGroup1Comp.RateGroupMemberOut[3] -> FileHandling.Subtopology.fileDownlinkRun
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[5] -> ComCcsdsSdls.Subtopology.comQueueRun
      rateGroup1Comp.RateGroupMemberOut[6] -> CdhCore.Subtopology.cmdDispRun
      rateGroup1Comp.RateGroupMemberOut[7] -> ComCcsdsSdls.Subtopology.aggregatorTimeout

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn
      rateGroup2Comp.RateGroupMemberOut[4] -> dpDemo.run
      #connection to FileManager listing feature command for sequencing
      rateGroup2Comp.RateGroupMemberOut[5] -> FileHandling.Subtopology.fileManagerSchedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> CdhCore.Subtopology.healthRun
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> ComCcsdsSdls.Subtopology.bufferManagerSchedIn
      rateGroup3Comp.RateGroupMemberOut[4] -> DataProducts.Subtopology.dpBufferManagerSchedIn
      rateGroup3Comp.RateGroupMemberOut[5] -> DataProducts.Subtopology.dpWriterSchedIn
      rateGroup3Comp.RateGroupMemberOut[6] -> DataProducts.Subtopology.dpMgrSchedIn
      rateGroup3Comp.RateGroupMemberOut[7] -> CdhCore.Subtopology.eventsRun
      #rateGroup3Comp.RateGroupMemberOut[8] -> DpCompression.Subtopology.dpZLibBufferManagerSchedIn
    }

    connections Communications {
      # ComDriver buffer allocations
      comDriver.allocate   -> ComCcsdsSdls.Subtopology.commsBufferGetCallee
      comDriver.deallocate -> ComCcsdsSdls.Subtopology.commsBufferSendIn

      # ComDriver <-> ComStub (Uplink)
      comDriver.$recv                          -> ComCcsdsSdls.Subtopology.drvReceiveIn
      ComCcsdsSdls.Subtopology.drvReceiveReturnOut -> comDriver.recvReturnIn

      # ComStub <-> ComDriver (Downlink)
      ComCcsdsSdls.Subtopology.drvSendOut -> comDriver.$send
      comDriver.ready                 -> ComCcsdsSdls.Subtopology.drvConnected
    }

    connections SdlsKeys {
      # Uplink and downlink are separate security associations but share one pre-shared key.
      ComCcsdsSdls.decryptor.keyGet -> ComCcsdsSdls.keyManager.keyGet
      ComCcsdsSdls.encryptor.keyGet -> ComCcsdsSdls.keyManager.keyGet
    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data

      ### Moved this out of DataProducts Subtopology --> anything specific to deployment should live in Ref connections
      # Synchronous request. Will have both request kinds for demo purposes, not typical
      SG1.productGetOut -> DataProducts.Subtopology.productGetIn
      # Asynchronous request
      SG1.productRequestOut -> DataProducts.Subtopology.productRequestIn
      DataProducts.Subtopology.productResponseOut -> SG1.productRecvIn
      # Send filled DP
      SG1.productSendOut -> DataProducts.Subtopology.productSendIn
      # Synchronous request
      dpDemo.productGetOut -> DataProducts.Subtopology.productGetIn
      # Send filled DP
      dpDemo.productSendOut -> DataProducts.Subtopology.productSendIn
      # Asynchronous request
      dpDemo.productRequestOut -> DataProducts.Subtopology.productRequestIn
      DataProducts.Subtopology.productResponseOut -> dpDemo.productRecvIn
    }

    connections ComCcsds_CdhCore {
      # Events and telemetry to comQueue
      CdhCore.Subtopology.eventsPktSend  -> ComCcsdsSdls.Subtopology.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.EVENTS]
      CdhCore.Subtopology.tlmSendPktSend -> ComCcsdsSdls.Subtopology.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.TELEMETRY]

      # Router <-> CmdDispatcher
      ComCcsdsSdls.Subtopology.commandOut        -> CdhCore.Subtopology.seqCmdBuff
      CdhCore.Subtopology.seqCmdStatus       -> ComCcsdsSdls.Subtopology.cmdResponseIn
      cmdSeq.comCmdOut                       -> CdhCore.Subtopology.seqCmdBuff
      CdhCore.Subtopology.seqCmdStatus       -> cmdSeq.cmdResponseIn
    }

    connections ComCcsds_FileHandling {
      # File Downlink <-> ComQueue
      FileHandling.Subtopology.fileDownlinkBufferSendOut -> ComCcsdsSdls.Subtopology.bufferQueueIn[ComCcsds.Ports_ComBufferQueue.FILE]
      ComCcsdsSdls.Subtopology.bufferReturnOut[ComCcsds.Ports_ComBufferQueue.FILE] -> FileHandling.Subtopology.fileDownlinkBufferReturn

      # Router <-> FileUplink
      ComCcsdsSdls.Subtopology.fileUplinkOut                    -> FileHandling.Subtopology.fileUplinkBufferSendIn
      FileHandling.Subtopology.fileUplinkBufferSendOut     -> ComCcsdsSdls.Subtopology.fileUplinkReturnIn
    }

    connections FileHandling_DataProducts {
      DataProducts.Subtopology.dpCatFileOut              -> FileHandling.Subtopology.fileDownlinkSendFile
      FileHandling.Subtopology.fileDownlinkFileComplete  -> DataProducts.Subtopology.dpCatFileDone

      #DataProducts.Subtopology.dpWriterProcOut[0] -> DpCompression.Subtopology.dpCompressProcIn
    }

  }

}
