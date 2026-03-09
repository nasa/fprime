module FppTest {

    topology Async {
        import Receiver

        instance sender1Async
        instance sender2Async

        connections AsyncReply {
            receiver1.replyOut[SenderId.ASYNC] -> sender1Async.replyIn
            receiver2.replyOut[SenderId.ASYNC] -> sender2Async.replyIn
        }

        connections AsyncInstance1 {
            sender1Async.noArgsOut[0] -> receiver1.noArgsAsync[0]
            sender1Async.noArgsOut[1] -> receiver1.noArgsAsync[1]

            sender1Async.primitiveArgsOut[0] -> receiver1.primitiveArgsAsync[0]
            sender1Async.primitiveArgsOut[1] -> receiver1.primitiveArgsAsync[1]

            sender1Async.stringArgsOut[0] -> receiver1.stringArgsAsync[0]
            sender1Async.stringArgsOut[1] -> receiver1.stringArgsAsync[1]

            sender1Async.enumArgsOut[0] -> receiver1.enumArgsAsync[0]
            sender1Async.enumArgsOut[1] -> receiver1.enumArgsAsync[1]

            sender1Async.arrayArgsOut[0] -> receiver1.arrayArgsAsync[0]
            sender1Async.arrayArgsOut[1] -> receiver1.arrayArgsAsync[1]

            sender1Async.structArgsOut[0] -> receiver1.structArgsAsync[0]
            sender1Async.structArgsOut[1] -> receiver1.structArgsAsync[1]
        }

        connections AsyncInstance2 {
            sender2Async.noArgsOut[0] -> receiver2.noArgsAsync[0]
            sender2Async.noArgsOut[1] -> receiver2.noArgsAsync[1]

            sender2Async.primitiveArgsOut[0] -> receiver2.primitiveArgsAsync[0]
            sender2Async.primitiveArgsOut[1] -> receiver2.primitiveArgsAsync[1]

            sender2Async.stringArgsOut[0] -> receiver2.stringArgsAsync[0]
            sender2Async.stringArgsOut[1] -> receiver2.stringArgsAsync[1]

            sender2Async.enumArgsOut[0] -> receiver2.enumArgsAsync[0]
            sender2Async.enumArgsOut[1] -> receiver2.enumArgsAsync[1]

            sender2Async.arrayArgsOut[0] -> receiver2.arrayArgsAsync[0]
            sender2Async.arrayArgsOut[1] -> receiver2.arrayArgsAsync[1]

            sender2Async.structArgsOut[0] -> receiver2.structArgsAsync[0]
            sender2Async.structArgsOut[1] -> receiver2.structArgsAsync[1]
        }
    }

}