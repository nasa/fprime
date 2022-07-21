module Ref {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
      rateGroup2
      rateGroup3
    }

    enum Ports_StaticMemory {
      downlink
      uplink
    }

  topology Ref {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance SG1
    instance SG2
    instance SG3
    instance SG4
    instance SG5
    instance blockDrv
    instance chanTlm
    instance cmdDisp
    instance cmdSeq
    instance comDriver
    instance comQueue
    instance comStub
    instance framer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileManager
    instance fileUplink
    instance fileUplinkBufferManager
    instance linuxTime
    instance pingRcvr
    instance prmDb
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance recvBuffComp
    instance sendBuffComp
    instance staticMemory
    instance textLogger
    instance deframer
    instance systemResources

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    param connections instance prmDb

    telemetry connections instance chanTlm

    text event connections instance textLogger

    time connections instance linuxTime

    health connections instance $health

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {
      chanTlm.PktSend -> comQueue.comQueueIn[0]
      eventLogger.PktSend -> comQueue.comQueueIn[1]

      fileDownlink.bufferSendOut -> comQueue.buffQueueIn[0]
      framer.bufferDeallocate -> fileDownlink.bufferReturn

      comQueue.comQueueSend -> framer.comIn
      comQueue.buffQueueSend -> framer.bufferIn

      framer.framedAllocate -> staticMemory.bufferAllocate[Ports_StaticMemory.downlink]
      framer.framedOut -> comStub.comDataIn
      comDriver.deallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.downlink]

      comStub.drvDataOut -> comDriver.send
      comDriver.ready -> comStub.drvConnected
      comStub.comStatus -> comQueue.comStatusIn
    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections RateGroups {

      # Block driver
      blockDrv.CycleOut -> rateGroupDriverComp.CycleIn

      # Rate group 1
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut[0] -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> chanTlm.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> fileDownlink.Run
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> $health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> fileUplinkBufferManager.schedIn

    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data
    }

    connections Sequencer {
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
    }
    connections Uplink {
      comDriver.allocate -> staticMemory.bufferAllocate[Ports_StaticMemory.uplink]
      comDriver.$recv -> comStub.drvDataIn

      comStub.comDataOut -> deframer.framedIn
      deframer.framedDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.uplink]

      deframer.comOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> deframer.cmdResponseIn

      deframer.bufferAllocate -> fileUplinkBufferManager.bufferGetCallee
      deframer.bufferOut -> fileUplink.bufferSendIn
      deframer.bufferDeallocate -> fileUplinkBufferManager.bufferSendIn
      fileUplink.bufferSendOut -> fileUplinkBufferManager.bufferSendIn
    }
  }
}
