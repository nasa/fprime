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

  instance pingRcvr: Ref.PingReceiver base id 0x0A00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 100

  instance typeDemo: Ref.TypeDemo base id 0x1200

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

  instance posixTime: Svc.PosixTime base id 0x4300

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x4400

  instance recvBuffComp: Ref.RecvBuff base id 0x4500

  instance systemResources: Svc.SystemResources base id 0x4600

  instance linuxTimer: Svc.LinuxTimer base id 0x5000

}
