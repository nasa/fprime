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

  instance sender1: Sender base id 0x1000

  instance sender2: Sender base id 0x1100

  instance receiver1: Receiver base id 0x1200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

  instance receiver2: Receiver base id 0x1300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

}
