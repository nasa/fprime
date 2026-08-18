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

    port serialReplyIn1 = sender1Top.serialReplyIn
    port serialReplyIn2 = sender2Top.serialReplyIn

    port serialReplyOut1 = sender1Top.serialReplyIn
    port serialReplyOut2 = sender2Top.serialReplyIn

    port serialOut1 = sender1Top.serialOut
    port serialOut2 = sender2Top.serialOut
  }

  topology ReceiverTop {
    instance receiver1Top
    instance receiver2Top

    port primitiveArgsSync1 = receiver1Top.primitiveArgsSync
    port primitiveArgsSync2 = receiver2Top.primitiveArgsSync

    port replyOut1 = receiver1Top.replyOut
    port replyOut2 = receiver2Top.replyOut

    port serialIn1 = receiver1Top.serialIn
    port serialIn2 = receiver2Top.serialIn

    port serialReplyOut1 = receiver1Top.serialReplyOut
    port serialReplyOut2 = receiver2Top.serialReplyOut
  }

  topology TopPorts {
    instance SenderTop
    instance ReceiverTop

    connections Top2Top {
        SenderTop.primitiveArgsOut1[0] -> ReceiverTop.primitiveArgsSync1[0]
        SenderTop.primitiveArgsOut1[1] -> ReceiverTop.primitiveArgsSync1[1]
        SenderTop.primitiveArgsOut1[2] -> ReceiverTop.serialIn1[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]

        SenderTop.primitiveArgsOut2[0] -> ReceiverTop.primitiveArgsSync2[0]
        SenderTop.primitiveArgsOut2[1] -> ReceiverTop.primitiveArgsSync2[1]
        SenderTop.primitiveArgsOut2[2] -> ReceiverTop.serialIn2[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]

        ReceiverTop.replyOut1[SenderId.SYNC] -> SenderTop.replyIn1
        ReceiverTop.replyOut2[SenderId.SYNC] -> SenderTop.replyIn2

        ReceiverTop.serialReplyOut1[SenderId.SYNC] -> SenderTop.serialReplyIn1
        ReceiverTop.serialReplyOut2[SenderId.SYNC] -> SenderTop.serialReplyIn2

        SenderTop.serialOut1[TestDeploymentPort.PRIMITIVE_ARGS_SYNC] -> ReceiverTop.primitiveArgsSync1[2]
        SenderTop.serialOut2[TestDeploymentPort.PRIMITIVE_ARGS_SYNC] -> ReceiverTop.primitiveArgsSync2[2]
    }

    connections SerialToSerialTest {
      SenderTop.serialOut1[TestDeploymentPort.SERIAL] -> ReceiverTop.serialIn1[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]
      SenderTop.serialOut2[TestDeploymentPort.SERIAL] -> ReceiverTop.serialIn2[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]
    }
  }

}
