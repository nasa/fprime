module Ref {

  # ----------------------------------------------------------------------
  # Module Instances
  # ----------------------------------------------------------------------

  @ Core C&DH Components
  instance cmdDisp: Svc.CommandDispatcher base id 121 \
    queue size 20 \
    stack size 10 * 1024 \
    priority 101

  instance chanTlm: Svc.TlmChan base id 61 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 97

  instance prmDb: Svc.PrmDb base id 141 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 96

  instance cmdSeq: Svc.CmdSequencer base id 541 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 100

  instance eventLogger: Svc.ActiveLogger base id 421 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 98

  instance $health: Svc.Health base id 361 \
    queue size 25 

  instance fileUplink: Svc.FileUplink base id 261 \
    queue size 30 \
    stack size 10 * 1024 \
    priority 100

  instance fileUplinkBufferManager: Svc.BufferManager base id 301

  instance fileDownlink: Svc.FileDownlink base id 501 \
    queue size 30 \
    stack size 10 * 1024 \
    priority 100

  instance fileManager: Svc.FileManager base id 601 \
    queue size 30 \
    stack size 10 * 1024 \
    priority 100

  instance textLogger: Svc.PassiveTextLogger base id 521

  instance fatalAdapter: Svc.AssertFatalAdapter base id 341

  instance fatalHandler: Svc.FatalHandler base id 1

  instance linuxTime: Svc.Time base id 441

  instance staticMemory: Svc.StaticMemory base id 661

  instance downlink: Svc.Framer base id 681

  instance uplink: Svc.Deframer base id 701

  @ Communications driver. May be swapped with other comm drivers like UART
  @ Note: here we have tcp reliable uplink, and Udp (low latency) downlink
  instance comm: Drv.ByteStreamDriverModel base id 621

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 461

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 21 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 120

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 161 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 119

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 241 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 118

  instance sendBuffComp: Ref.SendBuff base id 81 \
    queue size 10

  instance recvBuffComp: Ref.RecvBuff base id 101

  instance pingRcvr: Ref.PingReceiver base id 41 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 100

  instance SG1: Ref.SignalGen base id 181 \
    queue size 10 #instance 0

  instance SG3: Ref.SignalGen base id 201 \
    queue size 10 #instance 2

  instance SG2: Ref.SignalGen base id 221 \
    queue size 10 #instance 1

  instance SG5: Ref.SignalGen base id 281 \
    queue size 10 #instance 4

  instance SG4: Ref.SignalGen base id 321 \
    queue size 10 #instance 3

  instance blockDrv: Drv.BlockDriver base id 481 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 140

  topology Ref {

    # ----------------------------------------------------------------------
    # Topology Instances
    # ----------------------------------------------------------------------

    instance cmdDisp

    instance chanTlm

    instance prmDb

    instance cmdSeq

    instance eventLogger

    instance $health

    instance fileUplink

    instance fileUplinkBufferManager

    instance fileDownlink

    instance fileManager

    instance textLogger

    instance fatalAdapter

    instance fatalHandler

    instance linuxTime

    instance staticMemory

    instance downlink

    instance uplink

    instance comm

    instance rateGroupDriverComp

    instance rateGroup1Comp

    instance rateGroup2Comp

    instance rateGroup3Comp

    instance sendBuffComp

    instance recvBuffComp

    instance pingRcvr

    instance SG1

    instance SG3

    instance SG2

    instance SG5

    instance SG4

    instance blockDrv

    # ----------------------------------------------------------------------
    # Connections
    # ----------------------------------------------------------------------

    connections ComDrv {
      comm.allocate -> staticMemory.bufferAllocate
      comm.$recv -> uplink.framedIn
      uplink.framedDeallocate -> staticMemory.bufferDeallocate
    }
  
    connections UplinkData{
      uplink.bufferAllocate -> fileUplinkBufferManager.bufferGetCallee
      uplink.comOut -> cmdDisp.seqCmdBuff
      uplink.bufferOut -> fileUplink.bufferSendIn
      uplink.bufferDeallocate -> fileUplinkBufferManager.bufferSendIn
      fileUplink.bufferSendOut -> fileUplinkBufferManager.bufferSendIn
    }

    connections DownlinkPorts{
      downlink.framedAllocate -> staticMemory.bufferAllocate[1]
      downlink.framedOut -> comm.send
      comm.deallocate -> staticMemory.bufferDeallocate[1]
    }

    connections DownlinkData{
      eventLogger.PktSend -> downlink.comIn
      chanTlm.PktSend -> downlink.comIn
      fileDownlink.bufferSendOut -> downlink.bufferIn
      downlink.bufferDeallocate -> fileDownlink.bufferReturn
    }

    # no idea what event connections this refers to; doesnt change the code at all if included or left out
    #@ Event Connections
    #event connections instance eventLogger

    @ Command connections
    command connections instance cmdDisp

    @ Sequencer Connections - should not conflict with uplink port
    connections Sequencer{
      cmdDisp.seqCmdStatus[1] -> cmdSeq.cmdResponseIn
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff[1]
      prmDb.Log -> eventLogger.LogRecv
      fileUplink.eventOut -> eventLogger.LogRecv
      eventLogger.Log -> eventLogger.LogRecv
      fileUplinkBufferManager.eventOut -> eventLogger.LogRecv
      cmdDisp.Log -> eventLogger.LogRecv
      cmdSeq.logOut -> eventLogger.LogRecv
      fatalAdapter.Log -> eventLogger.LogRecv
      $health.Log -> eventLogger.LogRecv
      fileDownlink.eventOut -> eventLogger.LogRecv
      fileManager.eventOut -> eventLogger.LogRecv
      rateGroup1Comp.Log -> eventLogger.LogRecv
      rateGroup2Comp.Log -> eventLogger.LogRecv
      rateGroup3Comp.Log -> eventLogger.LogRecv
      sendBuffComp.Log -> eventLogger.LogRecv
      recvBuffComp.Log -> eventLogger.LogRecv
      SG1.logOut -> eventLogger.LogRecv
      SG2.logOut -> eventLogger.LogRecv
      SG3.logOut -> eventLogger.LogRecv
      SG4.logOut -> eventLogger.LogRecv
      SG5.logOut -> eventLogger.LogRecv
      pingRcvr.Log -> eventLogger.LogRecv
    }

    @ Text Event Connections
    text event connections instance textLogger

    #doesnt work because chanTlm has two input ports and it doesn't know which to use
    #@ Telemetry Connections
    #telemetry connections instance chanTlm

    connections Telemetry{
      fileDownlink.tlmOut -> chanTlm.TlmRecv
      fileUplinkBufferManager.tlmOut -> chanTlm.TlmRecv
      fileUplink.tlmOut -> chanTlm.TlmRecv
      fileManager.tlmOut -> chanTlm.TlmRecv
      cmdSeq.tlmOut -> chanTlm.TlmRecv
      cmdDisp.Tlm -> chanTlm.TlmRecv
      rateGroup1Comp.Tlm -> chanTlm.TlmRecv
      rateGroup2Comp.Tlm -> chanTlm.TlmRecv
      rateGroup3Comp.Tlm -> chanTlm.TlmRecv
      $health.Tlm -> chanTlm.TlmRecv
      SG1.tlmOut -> chanTlm.TlmRecv
      SG2.tlmOut -> chanTlm.TlmRecv
      SG3.tlmOut -> chanTlm.TlmRecv
      SG4.tlmOut -> chanTlm.TlmRecv
      SG5.tlmOut -> chanTlm.TlmRecv
      pingRcvr.Tlm -> chanTlm.TlmRecv
      sendBuffComp.Tlm -> chanTlm.TlmRecv
      recvBuffComp.Tlm -> chanTlm.TlmRecv
      blockDrv.Tlm -> chanTlm.TlmRecv
    }

    @ Parameters Connections
    param connections instance prmDb

    @ Time Connections
    time connections instance linuxTime
    

    @ Rate Group Connections
    connections Linuxtimer{
      blockDrv.CycleOut -> rateGroupDriverComp.CycleIn
    }

    connections RateGroup1{
      rateGroupDriverComp.CycleOut -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> chanTlm.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> fileDownlink.Run
    }
    
    connections RateGroup2 {
      rateGroupDriverComp.CycleOut[1] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut -> cmdSeq.schedIn
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn
    }

    connections RateGroup3{
      rateGroupDriverComp.CycleOut[2] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut -> $health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> fileUplinkBufferManager.schedIn
    }
  
    @ The PingSend output port number should match the PingReturn input port number
    @ Each port number pair must be unique
    @ This order must match the pingEntries[] table in Ref/Top/Topology.cpp
    health connections instance $health

    connections SocketGroupSysCom{}

    @ Uplink connection to command dispatcher should not conflict with command sequencer
    connections Uplink{}

    connections Fault{
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections ReferenceApp{
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data
    }
  } 
}
