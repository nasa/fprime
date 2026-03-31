module FppTest {

  instance sender1Top: Sender base id 0x2010 queue size 1
  instance sender2Top: Sender base id 0x2020 queue size 1

  instance receiver1Top: Receiver base id 0x2100 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

  instance receiver2Top: Receiver base id 0x2200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 20

  topology SenderTop {
    instance sender1Top
    instance sender2Top

    port primitiveArgsOut1 = sender1Top.primitiveArgsOut
    port primitiveArgsOut2 = sender2Top.primitiveArgsOut

    port replyIn1 = sender1Top.replyIn
    port replyIn2 = sender2Top.replyIn
  }

  topology ReceiverTop {
    instance receiver1Top
    instance receiver2Top

    port primitiveArgsSync1 = receiver1Top.primitiveArgsSync
    port primitiveArgsSync2 = receiver2Top.primitiveArgsSync

    port replyOut1 = receiver1Top.replyOut
    port replyOut2 = receiver2Top.replyOut
  }

  topology TopPorts {
    instance SenderTop
    instance ReceiverTop

    connections Top2Top {
        SenderTop.primitiveArgsOut1[0] -> ReceiverTop.primitiveArgsSync1[0]
        SenderTop.primitiveArgsOut1[1] -> ReceiverTop.primitiveArgsSync1[1]

        SenderTop.primitiveArgsOut2[0] -> ReceiverTop.primitiveArgsSync2[0]
        SenderTop.primitiveArgsOut2[1] -> ReceiverTop.primitiveArgsSync2[1]

        ReceiverTop.replyOut1[SenderId.SYNC] -> SenderTop.replyIn1
        ReceiverTop.replyOut2[SenderId.SYNC] -> SenderTop.replyIn2
    }
  }

}
