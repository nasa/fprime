module FppTest {

    topology Guarded {
        import Receiver

        instance sender1Guarded
        instance sender2Guarded

        connections GuardedReply {
            receiver1.replyOut[SenderId.GUARDED] -> sender1Guarded.replyIn
            receiver2.replyOut[SenderId.GUARDED] -> sender2Guarded.replyIn

            receiver1.serialReplyOut[SenderId.GUARDED] -> sender1Guarded.serialReplyIn
            receiver2.serialReplyOut[SenderId.GUARDED] -> sender2Guarded.serialReplyIn
        }

        connections GuardedInstance1 {
            sender1Guarded.noArgsOut[0] -> receiver1.noArgsGuarded[0]
            sender1Guarded.noArgsOut[1] -> receiver1.noArgsGuarded[1]

            sender1Guarded.primitiveArgsOut[0] -> receiver1.primitiveArgsGuarded[0]
            sender1Guarded.primitiveArgsOut[1] -> receiver1.primitiveArgsGuarded[1]

            sender1Guarded.stringArgsOut[0] -> receiver1.stringArgsGuarded[0]
            sender1Guarded.stringArgsOut[1] -> receiver1.stringArgsGuarded[1]

            sender1Guarded.enumArgsOut[0] -> receiver1.enumArgsGuarded[0]
            sender1Guarded.enumArgsOut[1] -> receiver1.enumArgsGuarded[1]

            sender1Guarded.arrayArgsOut[0] -> receiver1.arrayArgsGuarded[0]
            sender1Guarded.arrayArgsOut[1] -> receiver1.arrayArgsGuarded[1]

            sender1Guarded.structArgsOut[0] -> receiver1.structArgsGuarded[0]
            sender1Guarded.structArgsOut[1] -> receiver1.structArgsGuarded[1]

            sender1Guarded.noArgsReturnOut[0] -> receiver1.noArgsReturnGuarded[0]
            sender1Guarded.noArgsReturnOut[1] -> receiver1.noArgsReturnGuarded[1]

            sender1Guarded.primitiveReturnOut[0] -> receiver1.primitiveReturnGuarded[0]
            sender1Guarded.primitiveReturnOut[1] -> receiver1.primitiveReturnGuarded[1]

            sender1Guarded.stringReturnOut[0] -> receiver1.stringReturnGuarded[0]
            sender1Guarded.stringReturnOut[1] -> receiver1.stringReturnGuarded[1]

            sender1Guarded.stringAliasReturnOut[0] -> receiver1.stringAliasReturnGuarded[0]
            sender1Guarded.stringAliasReturnOut[1] -> receiver1.stringAliasReturnGuarded[1]

            sender1Guarded.enumReturnOut[0] -> receiver1.enumReturnGuarded[0]
            sender1Guarded.enumReturnOut[1] -> receiver1.enumReturnGuarded[1]

            sender1Guarded.arrayReturnOut[0] -> receiver1.arrayReturnGuarded[0]
            sender1Guarded.arrayReturnOut[1] -> receiver1.arrayReturnGuarded[1]

            sender1Guarded.arrayStringAliasReturnOut[0] -> receiver1.arrayStringAliasReturnGuarded[0]
            sender1Guarded.arrayStringAliasReturnOut[1] -> receiver1.arrayStringAliasReturnGuarded[1]

            sender1Guarded.structReturnOut[0] -> receiver1.structReturnGuarded[0]
            sender1Guarded.structReturnOut[1] -> receiver1.structReturnGuarded[1]
        }

        connections GuardedInstance2 {
            sender2Guarded.noArgsOut[0] -> receiver2.noArgsGuarded[0]
            sender2Guarded.noArgsOut[1] -> receiver2.noArgsGuarded[1]

            sender2Guarded.primitiveArgsOut[0] -> receiver2.primitiveArgsGuarded[0]
            sender2Guarded.primitiveArgsOut[1] -> receiver2.primitiveArgsGuarded[1]

            sender2Guarded.stringArgsOut[0] -> receiver2.stringArgsGuarded[0]
            sender2Guarded.stringArgsOut[1] -> receiver2.stringArgsGuarded[1]

            sender2Guarded.enumArgsOut[0] -> receiver2.enumArgsGuarded[0]
            sender2Guarded.enumArgsOut[1] -> receiver2.enumArgsGuarded[1]

            sender2Guarded.arrayArgsOut[0] -> receiver2.arrayArgsGuarded[0]
            sender2Guarded.arrayArgsOut[1] -> receiver2.arrayArgsGuarded[1]

            sender2Guarded.structArgsOut[0] -> receiver2.structArgsGuarded[0]
            sender2Guarded.structArgsOut[1] -> receiver2.structArgsGuarded[1]

            sender2Guarded.noArgsReturnOut[0] -> receiver2.noArgsReturnGuarded[0]
            sender2Guarded.noArgsReturnOut[1] -> receiver2.noArgsReturnGuarded[1]

            sender2Guarded.primitiveReturnOut[0] -> receiver2.primitiveReturnGuarded[0]
            sender2Guarded.primitiveReturnOut[1] -> receiver2.primitiveReturnGuarded[1]

            sender2Guarded.stringReturnOut[0] -> receiver2.stringReturnGuarded[0]
            sender2Guarded.stringReturnOut[1] -> receiver2.stringReturnGuarded[1]

            sender2Guarded.stringAliasReturnOut[0] -> receiver2.stringAliasReturnGuarded[0]
            sender2Guarded.stringAliasReturnOut[1] -> receiver2.stringAliasReturnGuarded[1]

            sender2Guarded.enumReturnOut[0] -> receiver2.enumReturnGuarded[0]
            sender2Guarded.enumReturnOut[1] -> receiver2.enumReturnGuarded[1]

            sender2Guarded.arrayReturnOut[0] -> receiver2.arrayReturnGuarded[0]
            sender2Guarded.arrayReturnOut[1] -> receiver2.arrayReturnGuarded[1]

            sender2Guarded.arrayStringAliasReturnOut[0] -> receiver2.arrayStringAliasReturnGuarded[0]
            sender2Guarded.arrayStringAliasReturnOut[1] -> receiver2.arrayStringAliasReturnGuarded[1]

            sender2Guarded.structReturnOut[0] -> receiver2.structReturnGuarded[0]
            sender2Guarded.structReturnOut[1] -> receiver2.structReturnGuarded[1]
        }

        connections GuardedSerialInstance1 {
            sender1Guarded.serialOut[TestDeploymentPort.NO_ARGS_GUARDED] -> receiver1.noArgsGuarded[2]
            sender1Guarded.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_GUARDED] -> receiver1.primitiveArgsGuarded[2]
            sender1Guarded.serialOut[TestDeploymentPort.STRING_ARGS_GUARDED] -> receiver1.stringArgsGuarded[2]
            sender1Guarded.serialOut[TestDeploymentPort.ENUM_ARGS_GUARDED] -> receiver1.enumArgsGuarded[2]
            sender1Guarded.serialOut[TestDeploymentPort.ARRAY_ARGS_GUARDED] -> receiver1.arrayArgsGuarded[2]
            sender1Guarded.serialOut[TestDeploymentPort.STRUCT_ARGS_GUARDED] -> receiver1.structArgsGuarded[2]

            sender1Guarded.noArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.NO_ARGS_GUARDED]
            sender1Guarded.primitiveArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_GUARDED]
            sender1Guarded.stringArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRING_ARGS_GUARDED]
            sender1Guarded.enumArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ENUM_ARGS_GUARDED]
            sender1Guarded.arrayArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ARRAY_ARGS_GUARDED]
            sender1Guarded.structArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRUCT_ARGS_GUARDED]
        }

        connections GuardedSerialInstance2 {
            sender2Guarded.serialOut[TestDeploymentPort.NO_ARGS_GUARDED] -> receiver2.noArgsGuarded[2]
            sender2Guarded.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_GUARDED] -> receiver2.primitiveArgsGuarded[2]
            sender2Guarded.serialOut[TestDeploymentPort.STRING_ARGS_GUARDED] -> receiver2.stringArgsGuarded[2]
            sender2Guarded.serialOut[TestDeploymentPort.ENUM_ARGS_GUARDED] -> receiver2.enumArgsGuarded[2]
            sender2Guarded.serialOut[TestDeploymentPort.ARRAY_ARGS_GUARDED] -> receiver2.arrayArgsGuarded[2]
            sender2Guarded.serialOut[TestDeploymentPort.STRUCT_ARGS_GUARDED] -> receiver2.structArgsGuarded[2]

            sender2Guarded.noArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.NO_ARGS_GUARDED]
            sender2Guarded.primitiveArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_GUARDED]
            sender2Guarded.stringArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRING_ARGS_GUARDED]
            sender2Guarded.enumArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ENUM_ARGS_GUARDED]
            sender2Guarded.arrayArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ARRAY_ARGS_GUARDED]
            sender2Guarded.structArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRUCT_ARGS_GUARDED]
        }
    }

}