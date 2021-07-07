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

    @ Command Registration/Dispatch/Reply Connections
    @ Command Registration Ports - Registration port number must match dispatch port for each component
    connections ComRegPorts {
      eventLogger.CmdReg -> cmdDisp.compCmdReg
      cmdDisp.CmdReg -> cmdDisp.compCmdReg[1]
      $health.CmdReg -> cmdDisp.compCmdReg[2]
      prmDb.CmdReg -> cmdDisp.compCmdReg[3]
      cmdSeq.cmdRegOut -> cmdDisp.compCmdReg[4]
      fileDownlink.cmdRegOut -> cmdDisp.compCmdReg[5]
      sendBuffComp.CmdReg -> cmdDisp.compCmdReg[6]
      recvBuffComp.CmdReg -> cmdDisp.compCmdReg[7]
      SG1.cmdRegOut -> cmdDisp.compCmdReg[8]
      SG2.cmdRegOut -> cmdDisp.compCmdReg[9]
      SG3.cmdRegOut -> cmdDisp.compCmdReg[10]
      SG4.cmdRegOut -> cmdDisp.compCmdReg[11]
      SG5.cmdRegOut -> cmdDisp.compCmdReg[12]
      pingRcvr.CmdReg -> cmdDisp.compCmdReg[13]
      fileManager.cmdRegOut -> cmdDisp.compCmdReg[14]
    }

    @ Command Dispatch Ports - Dispatch port number must match registration port for each component
    connections CmdDispatch {
      cmdDisp.compCmdSend -> eventLogger.CmdDisp
      cmdDisp.compCmdSend[1] -> cmdDisp.CmdDisp
      cmdDisp.compCmdSend[2] -> $health.CmdDisp
      cmdDisp.compCmdSend[3] -> prmDb.CmdDisp
      cmdDisp.compCmdSend[4] -> cmdSeq.cmdIn
      cmdDisp.compCmdSend[5] -> fileDownlink.cmdIn
      cmdDisp.compCmdSend[6] -> sendBuffComp.CmdDisp
      cmdDisp.compCmdSend[7] -> recvBuffComp.CmdDisp
      cmdDisp.compCmdSend[8] -> SG1.cmdIn
      cmdDisp.compCmdSend[9] -> SG2.cmdIn
      cmdDisp.compCmdSend[10] -> SG3.cmdIn
      cmdDisp.compCmdSend[11] -> SG4.cmdIn
      cmdDisp.compCmdSend[12] -> SG5.cmdIn
      cmdDisp.compCmdSend[13] -> pingRcvr.CmdDisp
      cmdDisp.compCmdSend[14] -> fileManager.cmdIn
    }

    @ Command Reply Ports - Go to the same response port on the dispatcher
    connections ComReply {
      eventLogger.CmdStatus -> cmdDisp.compCmdStat
      cmdSeq.cmdResponseOut -> cmdDisp.compCmdStat
      prmDb.CmdStatus -> cmdDisp.compCmdStat
      cmdDisp.CmdStatus -> cmdDisp.compCmdStat
      $health.CmdStatus -> cmdDisp.compCmdStat
      fileDownlink.cmdResponseOut -> cmdDisp.compCmdStat
      fileManager.cmdResponseOut -> cmdDisp.compCmdStat
      pingRcvr.CmdStatus -> cmdDisp.compCmdStat
      recvBuffComp.CmdStatus -> cmdDisp.compCmdStat
      sendBuffComp.CmdStatus -> cmdDisp.compCmdStat
      SG1.cmdResponseOut -> cmdDisp.compCmdStat
      SG2.cmdResponseOut -> cmdDisp.compCmdStat
      SG3.cmdResponseOut -> cmdDisp.compCmdStat
      SG4.cmdResponseOut -> cmdDisp.compCmdStat
      SG5.cmdResponseOut -> cmdDisp.compCmdStat
    }

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


    connections TextEventLogger{
      cmdSeq.LogText -> textLogger.TextLogger
      eventLogger.LogText -> textLogger.TextLogger
      $health.LogText -> textLogger.TextLogger
      fatalAdapter.LogText -> textLogger.TextLogger
      prmDb.LogText -> textLogger.TextLogger
      cmdDisp.LogText -> textLogger.TextLogger
      rateGroup1Comp.LogText -> textLogger.TextLogger
      rateGroup2Comp.LogText -> textLogger.TextLogger
      rateGroup3Comp.LogText -> textLogger.TextLogger
      pingRcvr.LogText -> textLogger.TextLogger
      SG1.logTextOut -> textLogger.TextLogger
      SG2.logTextOut -> textLogger.TextLogger
      SG3.logTextOut -> textLogger.TextLogger
      SG4.logTextOut -> textLogger.TextLogger
      SG5.logTextOut -> textLogger.TextLogger
      sendBuffComp.LogText -> textLogger.TextLogger
      recvBuffComp.LogText -> textLogger.TextLogger
      fileDownlink.textEventOut -> textLogger.TextLogger
    }

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

    connections Parameters{
      recvBuffComp.ParamGet -> prmDb.getPrm
      sendBuffComp.ParamGet -> prmDb.getPrm
      recvBuffComp.ParamSet -> prmDb.setPrm
      sendBuffComp.ParamSet -> prmDb.setPrm
    }

    #time pattern
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
    connections Health{
      rateGroup1Comp.PingOut -> $health.PingReturn
      $health.PingSend -> rateGroup1Comp.PingIn
      $health.PingSend[1] -> rateGroup2Comp.PingIn
      rateGroup2Comp.PingOut -> $health.PingReturn[1]
      $health.PingSend[2] -> rateGroup3Comp.PingIn
      rateGroup3Comp.PingOut -> $health.PingReturn[2]
      $health.PingSend[3] -> cmdDisp.pingIn
      cmdDisp.pingOut -> $health.PingReturn[3]
      $health.PingSend[4] -> eventLogger.pingIn
      eventLogger.pingOut -> $health.PingReturn[4]
      $health.PingSend[5] -> cmdSeq.pingIn
      cmdSeq.pingOut -> $health.PingReturn[5]
      $health.PingSend[6] -> chanTlm.pingIn
      chanTlm.pingOut -> $health.PingReturn[6]
      $health.PingSend[7] -> prmDb.pingIn
      prmDb.pingOut -> $health.PingReturn[7]
      $health.PingSend[8] -> fileUplink.pingIn
      fileUplink.pingOut -> $health.PingReturn[8]
      $health.PingSend[9] -> fileDownlink.pingIn
      fileDownlink.pingOut -> $health.PingReturn[9]
      $health.PingSend[10] -> pingRcvr.PingIn
      pingRcvr.PingOut -> $health.PingReturn[10]
      $health.PingSend[11] -> blockDrv.PingIn
      blockDrv.PingOut -> $health.PingReturn[11]
      $health.PingSend[12] -> fileManager.pingIn
      fileManager.pingOut -> $health.PingReturn[12]
    }
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
