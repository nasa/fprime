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

  instance sendBuffComp: Ref.SendBuff base id 0x2000 \
    queue size Default.QUEUE_SIZE

  instance SG1: Ref.SignalGen base id 0x2100 \
    queue size Default.QUEUE_SIZE

  instance SG2: Ref.SignalGen base id 0x2200 \
    queue size Default.QUEUE_SIZE

  instance SG3: Ref.SignalGen base id 0x2300 \
    queue size Default.QUEUE_SIZE

  instance SG4: Ref.SignalGen base id 0x2400 \
    queue size Default.QUEUE_SIZE

  instance SG5: Ref.SignalGen base id 0x2500 \
    queue size Default.QUEUE_SIZE

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  @ Communications driver. May be swapped with other comm drivers like UART
  instance comDriver: Drv.TcpClient base id 0x4100

  instance commsBufferManager: Svc.BufferManager base id 0x4200

  instance posixTime: Svc.PosixTime base id 0x4300

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x4400

  instance recvBuffComp: Ref.RecvBuff base id 0x4500

  instance systemResources: Svc.SystemResources base id 0x4600

  instance dpBufferManager: Svc.BufferManager base id 0x4700

  instance frameAccumulator: Svc.FrameAccumulator base id 0x4800

  instance tcDeframer: Svc.Ccsds.TcDeframer base id 0x4900

  instance spacePacketDeframer: Svc.Ccsds.SpacePacketDeframer base id 0x4A00

  instance tmFramer: Svc.Ccsds.TmFramer base id 0x4B00

  instance spacePacketFramer: Svc.Ccsds.SpacePacketFramer base id 0x4C00

  instance fprimeRouter: Svc.FprimeRouter base id 0x4D00

  instance apidManager: Svc.Ccsds.ApidManager base id 0x4E00

  instance comStub: Svc.ComStub base id 0x4F00

  instance linuxTimer: Svc.LinuxTimer base id 0x5000


}
