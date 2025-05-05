module Ref {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
    rateGroup3
  }

  enum Ports_ComPacketQueue {
    EVENTS,
    TELEMETRY
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
    instance tlmSend
    instance cmdDisp
    instance cmdSeq
    instance comDriver
    instance comStub
    instance comQueue
    instance deframer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileManager
    instance fileUplink
    instance commsBufferManager
    instance frameAccumulator
    instance fprimeFramer
    instance posixTime
    instance pingRcvr
    instance prmDb
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance recvBuffComp
    instance fprimeRouter
    instance sendBuffComp
    instance textLogger
    instance typeDemo
    instance systemResources
    instance dpCat
    instance dpMgr
    instance dpWriter
    instance dpBufferManager
    instance version

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    param connections instance prmDb

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance posixTime

    health connections instance $health

    # ----------------------------------------------------------------------
    # Telemetry packets
    # ----------------------------------------------------------------------

    include "RefPackets.fppi"

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {
      dpCat.fileOut -> fileDownlink.SendFile
      fileDownlink.FileComplete -> dpCat.fileDone

      eventLogger.PktSend -> comQueue.comPacketQueueIn[Ports_ComPacketQueue.EVENTS]
      tlmSend.PktSend -> comQueue.comPacketQueueIn[Ports_ComPacketQueue.TELEMETRY]
      fileDownlink.bufferSendOut -> comQueue.bufferQueueIn[0]
      comQueue.bufferReturnOut[0] -> fileDownlink.bufferReturn

      comQueue.queueSend -> fprimeFramer.dataIn
      fprimeFramer.dataReturnOut -> comQueue.bufferReturnIn
      fprimeFramer.comStatusOut -> comQueue.comStatusIn

      fprimeFramer.bufferAllocate -> commsBufferManager.bufferGetCallee
      fprimeFramer.bufferDeallocate -> commsBufferManager.bufferSendIn

      fprimeFramer.dataOut -> comStub.comDataIn
      comStub.dataReturnOut -> fprimeFramer.dataReturnIn
      comStub.comStatusOut -> fprimeFramer.comStatusIn

      comStub.drvDataOut -> comDriver.$send
      comDriver.dataReturnOut -> comStub.dataReturnIn
      comDriver.ready -> comStub.drvConnected
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
      rateGroup1Comp.RateGroupMemberOut[2] -> tlmSend.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> fileDownlink.Run
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[5] -> comQueue.run

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
      rateGroup3Comp.RateGroupMemberOut[3] -> commsBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[4] -> dpBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[5] -> dpWriter.schedIn
      rateGroup3Comp.RateGroupMemberOut[6] -> dpMgr.schedIn
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

      comDriver.allocate -> commsBufferManager.bufferGetCallee
      comDriver.$recv -> comStub.drvDataIn
      comStub.comDataOut -> frameAccumulator.dataIn

      frameAccumulator.frameOut -> deframer.framedIn
      frameAccumulator.bufferAllocate -> commsBufferManager.bufferGetCallee
      frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
      deframer.bufferDeallocate -> commsBufferManager.bufferSendIn
      deframer.deframedOut -> fprimeRouter.dataIn

      fprimeRouter.commandOut -> cmdDisp.seqCmdBuff
      fprimeRouter.fileOut -> fileUplink.bufferSendIn
      fprimeRouter.bufferDeallocate -> commsBufferManager.bufferSendIn

      cmdDisp.seqCmdStatus -> fprimeRouter.cmdResponseIn

      fileUplink.bufferSendOut -> commsBufferManager.bufferSendIn

    }

    connections DataProducts {
      # DpMgr and DpWriter connections. Have explicit port indexes for demo
      dpMgr.bufferGetOut[0] -> dpBufferManager.bufferGetCallee
      dpMgr.productSendOut[0] -> dpWriter.bufferSendIn
      dpWriter.deallocBufferSendOut -> dpBufferManager.bufferSendIn

      # Component DP connections

      # Synchronous request. Will have both request kinds for demo purposes, not typical
      SG1.productGetOut -> dpMgr.productGetIn[0]
      # Asynchronous request
      SG1.productRequestOut -> dpMgr.productRequestIn[0]
      dpMgr.productResponseOut[0] -> SG1.productRecvIn
      # Send filled DP
      SG1.productSendOut -> dpMgr.productSendIn[0]

    }

  }

}
