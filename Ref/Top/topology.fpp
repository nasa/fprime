module Ref {

  topology Ref {

    # ----------------------------------------------------------------------
    # Instance specifiers
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
    instance comm
    instance downlink
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
    instance uplink

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
      downlink.framedAllocate -> staticMemory.bufferAllocate[1]
      downlink.framedOut -> comm.send
      comm.deallocate -> staticMemory.bufferDeallocate[1]
      eventLogger.PktSend -> downlink.comIn
      chanTlm.PktSend -> downlink.comIn
      fileDownlink.bufferSendOut -> downlink.bufferIn
      downlink.bufferDeallocate -> fileDownlink.bufferReturn
    }

    connections RateGroups {

      blockDrv.CycleOut -> rateGroupDriverComp.CycleIn

      rateGroupDriverComp.CycleOut -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> chanTlm.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> fileDownlink.Run

      rateGroupDriverComp.CycleOut[1] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn

      rateGroupDriverComp.CycleOut[2] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut -> $health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> fileUplinkBufferManager.schedIn

    }
  
    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data
    }

    connections Sequencer {
      # Should not conflict with uplink port
      cmdDisp.seqCmdStatus[1] -> cmdSeq.cmdResponseIn
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff[1]
    }

    connections Uplink {
      comm.allocate -> staticMemory.bufferAllocate
      comm.$recv -> uplink.framedIn
      uplink.framedDeallocate -> staticMemory.bufferDeallocate
      uplink.bufferAllocate -> fileUplinkBufferManager.bufferGetCallee
      # Uplink connection to command dispatcher should not conflict with command sequencer
      uplink.comOut -> cmdDisp.seqCmdBuff
      uplink.bufferOut -> fileUplink.bufferSendIn
      uplink.bufferDeallocate -> fileUplinkBufferManager.bufferSendIn
      fileUplink.bufferSendOut -> fileUplinkBufferManager.bufferSendIn
    }

  } 

}
