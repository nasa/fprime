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

  instance blockDrv: Ref.BlockDriver base id 0x10000000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 140

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 0x10001000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 120

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 0x10002000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 119

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 0x10003000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 118

  instance pingRcvr: Ref.PingReceiver base id 0x10004000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 100

  instance typeDemo: Ref.TypeDemo base id 0x10005000

  instance cmdSeq: Svc.CmdSequencer base id 0x10006000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 97

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  instance sendBuffComp: Ref.SendBuff base id 0x10007000 \
    queue size Default.QUEUE_SIZE

  instance SG1: Ref.SignalGen base id 0x10008000 \
    queue size Default.QUEUE_SIZE

  instance SG2: Ref.SignalGen base id 0x10009000 \
    queue size Default.QUEUE_SIZE

  instance SG3: Ref.SignalGen base id 0x1000A000 \
    queue size Default.QUEUE_SIZE

  instance SG4: Ref.SignalGen base id 0x1000B000 \
    queue size Default.QUEUE_SIZE

  instance SG5: Ref.SignalGen base id 0x1000C000 \
    queue size Default.QUEUE_SIZE

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  instance posixTime: Svc.PosixTime base id 0x1000D000

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x1000E000

  instance recvBuffComp: Ref.RecvBuff base id 0x1000F000

  instance systemResources: Svc.SystemResources base id 0x10010000

  instance linuxTimer: Svc.LinuxTimer base id 0x10011000

  instance comDriver: Drv.TcpClient base id 0x10012000 

}
