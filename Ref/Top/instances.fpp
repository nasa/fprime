module Ref {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {
    constant QUEUE_SIZE = 10
    constant STACK_SIZE = 64 * 1024
  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance blockDrv: Ref.BlockDriver base id 0x0100 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 140

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 0x0200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 120

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 0x0300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 119

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 0x0400 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 118

  instance cmdSeq: Svc.CmdSequencer base id 0x0500 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 100

  instance fileDownlink: Svc.FileDownlink base id 0x0600 \
    queue size 30 \
    stack size Default.STACK_SIZE \
    priority 100

  instance fileManager: Svc.FileManager base id 0x0700 \
    queue size 30 \
    stack size Default.STACK_SIZE \
    priority 100

  instance fileUplink: Svc.FileUplink base id 0x0800 \
    queue size 30 \
    stack size Default.STACK_SIZE \
    priority 100

  instance pingRcvr: Ref.PingReceiver base id 0x0900 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 100

  instance prmDb: Svc.PrmDb base id 0x0A00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 96

  instance dpCat: Svc.DpCatalog base id 0x0B00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 96

  instance dpMgr: Svc.DpManager base id 0x0C00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 96

  instance dpWriter: Svc.DpWriter base id 0x0D00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 96

  # ComQueue has a deeper queue to be resilient to spikes in com throughput
  instance comQueue: Svc.ComQueue base id 0x0E00 \
      queue size 50 \
      stack size Default.STACK_SIZE \
      priority 100

  instance typeDemo: Ref.TypeDemo base id 0x0F00

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  instance $health: Svc.Health base id 0x1000 \
    queue size 25

  instance SG1: Ref.SignalGen base id 0x1100 \
    queue size Default.QUEUE_SIZE

  instance SG2: Ref.SignalGen base id 0x1200 \
    queue size Default.QUEUE_SIZE

  instance SG3: Ref.SignalGen base id 0x1300 \
    queue size Default.QUEUE_SIZE

  instance SG4: Ref.SignalGen base id 0x1400 \
    queue size Default.QUEUE_SIZE

  instance SG5: Ref.SignalGen base id 0x1500 \
    queue size Default.QUEUE_SIZE

  instance sendBuffComp: Ref.SendBuff base id 0x1600 \
    queue size Default.QUEUE_SIZE
  
  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  @ Communications driver. May be swapped with other comm drivers like UART
  instance comDriver: Drv.TcpClient base id 0x1700

  instance fatalAdapter: Svc.AssertFatalAdapter base id 0x1800

  instance fatalHandler: Svc.FatalHandler base id 0x1900

  instance commsBufferManager: Svc.BufferManager base id 0x1A00

  instance posixTime: Svc.PosixTime base id 0x1B00

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x1C00

  instance recvBuffComp: Ref.RecvBuff base id 0x1D00

  instance version: Svc.Version base id 0x1E00

  instance textLogger: Svc.PassiveTextLogger base id 0x1F00

  instance systemResources: Svc.SystemResources base id 0x2000

  instance dpBufferManager: Svc.BufferManager base id 0x2100

  instance frameAccumulator: Svc.FrameAccumulator base id 0x2200

  instance tcDeframer: Svc.CCSDS.TcDeframer base id 0x2300

  instance spacePacketDeframer: Svc.CCSDS.SpacePacketDeframer base id 0x2400

  instance tmFramer: Svc.CCSDS.TmFramer base id 0x2500

  instance spacePacketFramer: Svc.CCSDS.SpacePacketFramer base id 0x2600

  instance fprimeRouter: Svc.FprimeRouter base id 0x2700

  instance apidManager: Svc.CCSDS.ApidManager base id 0x2800

  instance comStub: Svc.ComStub base id 0x2900

  instance linuxTimer: Svc.LinuxTimer base id 0x2A00

}
