module FppTest {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {
    constant QUEUE_SIZE = 10
    constant STACK_SIZE = 64 * 1024
  }

  # ----------------------------------------------------------------------
  # Component instances
  # ----------------------------------------------------------------------

  instance sender1Sync: Sender base id 0x1010
  instance sender1Guarded: Sender base id 0x1020
  instance sender1Async: Sender base id 0x1030

  instance sender2Sync: Sender base id 0x1110
  instance sender2Guarded: Sender base id 0x1120
  instance sender2Async: Sender base id 0x1130

  instance receiver1: Receiver base id 0x1200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

  instance receiver2: Receiver base id 0x1300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

}
