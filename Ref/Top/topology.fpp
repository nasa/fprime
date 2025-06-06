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

  enum Ports_ComBufferQueue {
    FILE_DOWNLINK
  }

  topology Ref {
    # ----------------------------------------------------------------------
    # Subtopology imports
    # ----------------------------------------------------------------------
    import CDHCore.Subtopology
    import Comms.Subtopology

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    #instance $health
    instance SG1
    instance SG2
    instance SG3
    instance SG4
    instance SG5
    instance blockDrv
    #instance tlmSend
    #instance cmdDisp
    instance cmdSeq
    #instance comDriver
    #instance comStub
    #instance comQueue
    #instance deframer
    #instance eventLogger
    #instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileManager
    instance fileUplink
    #instance Comms.commsBufferManager
    #instance frameAccumulator
    #instance fprimeFramer
    instance posixTime
    instance pingRcvr
    instance prmDb
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance recvBuffComp
    #instance fprimeRouter
    instance sendBuffComp
    #instance textLogger
    instance typeDemo
    instance systemResources
    instance dpCat
    instance dpMgr
    instance dpWriter
    instance dpBufferManager
    #instance version
    instance linuxTimer

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance CDHCore.cmdDisp

    event connections instance CDHCore.events

    param connections instance prmDb

    telemetry connections instance CDHCore.tlmSend

    text event connections instance CDHCore.textLogger

    time connections instance posixTime

    health connections instance CDHCore.$health

    # ----------------------------------------------------------------------
    # Telemetry packets
    # ----------------------------------------------------------------------

    include "RefPackets.fppi"

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections FaultProtection {
      CDHCore.events.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections RateGroups {

      # Linux timer to drive cycle
      linuxTimer.CycleOut -> rateGroupDriverComp.CycleIn

      # Rate group 1
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut[0] -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> CDHCore.tlmSend.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> fileDownlink.Run
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[5] -> Comms.comQueue.run

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> CDHCore.$health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> Comms.commsBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[4] -> dpBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[5] -> dpWriter.schedIn
      rateGroup3Comp.RateGroupMemberOut[6] -> dpMgr.schedIn
    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data
    }

    connections Sequencer {
      cmdSeq.comCmdOut -> CDHCore.cmdDisp.seqCmdBuff
      CDHCore.cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
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
