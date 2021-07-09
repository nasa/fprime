module Ref {

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
    queue size 10

  instance SG3: Ref.SignalGen base id 201 \
    queue size 10

  instance SG2: Ref.SignalGen base id 221 \
    queue size 10

  instance SG5: Ref.SignalGen base id 281 \
    queue size 10

  instance SG4: Ref.SignalGen base id 321 \
    queue size 10

  instance blockDrv: Drv.BlockDriver base id 481 \
    queue size 10 \
    stack size 10 * 1024 \
    priority 140

}
