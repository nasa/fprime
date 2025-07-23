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

  instance blockDrv: Ref.BlockDriver base id 0x01000000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 140

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 0x01010000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 120

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 0x01020000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 119

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 0x01030000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 118

  instance pingRcvr: Ref.PingReceiver base id 0x01040000 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 100

  instance typeDemo: Ref.TypeDemo base id 0x01050000

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  instance sendBuffComp: Ref.SendBuff base id 0x01060000 \
    queue size Default.QUEUE_SIZE

  instance SG1: Ref.SignalGen base id 0x01070000 \
    queue size Default.QUEUE_SIZE

  instance SG2: Ref.SignalGen base id 0x01080000 \
    queue size Default.QUEUE_SIZE

  instance SG3: Ref.SignalGen base id 0x01090000 \
    queue size Default.QUEUE_SIZE

  instance SG4: Ref.SignalGen base id 0x010A0000 \
    queue size Default.QUEUE_SIZE

  instance SG5: Ref.SignalGen base id 0x010B0000 \
    queue size Default.QUEUE_SIZE

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  instance posixTime: Svc.PosixTime base id 0x010C0000

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x010D0000

  instance recvBuffComp: Ref.RecvBuff base id 0x010E0000

  instance systemResources: Svc.SystemResources base id 0x010F0000

  instance linuxTimer: Svc.LinuxTimer base id 0x01100000

}
