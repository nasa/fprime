module FppTest {

    topology Async {
        import Receiver

        instance sender1Async
        instance sender2Async

        connections AsyncReply {
            receiver1.replyOut[SenderId.ASYNC] -> sender1Async.replyIn
            receiver2.replyOut[SenderId.ASYNC] -> sender2Async.replyIn

            receiver1.serialReplyOut[SenderId.ASYNC] -> sender1Async.serialReplyIn
            receiver2.serialReplyOut[SenderId.ASYNC] -> sender2Async.serialReplyIn
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

        connections AsyncSerialInstance1 {
            sender1Async.serialOut[TestDeploymentPort.NO_ARGS_ASYNC] -> receiver1.noArgsAsync[2]
            sender1Async.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_ASYNC] -> receiver1.primitiveArgsAsync[2]
            sender1Async.serialOut[TestDeploymentPort.STRING_ARGS_ASYNC] -> receiver1.stringArgsAsync[2]
            sender1Async.serialOut[TestDeploymentPort.ENUM_ARGS_ASYNC] -> receiver1.enumArgsAsync[2]
            sender1Async.serialOut[TestDeploymentPort.ARRAY_ARGS_ASYNC] -> receiver1.arrayArgsAsync[2]
            sender1Async.serialOut[TestDeploymentPort.STRUCT_ARGS_ASYNC] -> receiver1.structArgsAsync[2]

            sender1Async.noArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.NO_ARGS_ASYNC]
            sender1Async.primitiveArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_ASYNC]
            sender1Async.stringArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRING_ARGS_ASYNC]
            sender1Async.enumArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ENUM_ARGS_ASYNC]
            sender1Async.arrayArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ARRAY_ARGS_ASYNC]
            sender1Async.structArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRUCT_ARGS_ASYNC]
        }

        connections AsyncSerialInstance2 {
            sender2Async.serialOut[TestDeploymentPort.NO_ARGS_ASYNC] -> receiver2.noArgsAsync[2]
            sender2Async.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_ASYNC] -> receiver2.primitiveArgsAsync[2]
            sender2Async.serialOut[TestDeploymentPort.STRING_ARGS_ASYNC] -> receiver2.stringArgsAsync[2]
            sender2Async.serialOut[TestDeploymentPort.ENUM_ARGS_ASYNC] -> receiver2.enumArgsAsync[2]
            sender2Async.serialOut[TestDeploymentPort.ARRAY_ARGS_ASYNC] -> receiver2.arrayArgsAsync[2]
            sender2Async.serialOut[TestDeploymentPort.STRUCT_ARGS_ASYNC] -> receiver2.structArgsAsync[2]

            sender2Async.noArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.NO_ARGS_ASYNC]
            sender2Async.primitiveArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_ASYNC]
            sender2Async.stringArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRING_ARGS_ASYNC]
            sender2Async.enumArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ENUM_ARGS_ASYNC]
            sender2Async.arrayArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ARRAY_ARGS_ASYNC]
            sender2Async.structArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRUCT_ARGS_ASYNC]
        }
    }

}