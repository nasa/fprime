module FppTest {

    topology Sync {
        import Receiver

        instance sender1Sync
        instance sender2Sync

        connections SyncReply {
            receiver1.replyOut[SenderId.SYNC] -> sender1Sync.replyIn
            receiver2.replyOut[SenderId.SYNC] -> sender2Sync.replyIn

            receiver1.serialReplyOut[SenderId.SYNC] -> sender1Sync.serialReplyIn
            receiver2.serialReplyOut[SenderId.SYNC] -> sender2Sync.serialReplyIn
        }

        connections SyncInstance1 {
            sender1Sync.noArgsOut[0] -> receiver1.noArgsSync[0]
            sender1Sync.noArgsOut[1] -> receiver1.noArgsSync[1]

            sender1Sync.primitiveArgsOut[0] -> receiver1.primitiveArgsSync[0]
            sender1Sync.primitiveArgsOut[1] -> receiver1.primitiveArgsSync[1]

            sender1Sync.stringArgsOut[0] -> receiver1.stringArgsSync[0]
            sender1Sync.stringArgsOut[1] -> receiver1.stringArgsSync[1]

            sender1Sync.enumArgsOut[0] -> receiver1.enumArgsSync[0]
            sender1Sync.enumArgsOut[1] -> receiver1.enumArgsSync[1]

            sender1Sync.arrayArgsOut[0] -> receiver1.arrayArgsSync[0]
            sender1Sync.arrayArgsOut[1] -> receiver1.arrayArgsSync[1]

            sender1Sync.structArgsOut[0] -> receiver1.structArgsSync[0]
            sender1Sync.structArgsOut[1] -> receiver1.structArgsSync[1]

            sender1Sync.noArgsReturnOut[0] -> receiver1.noArgsReturnSync[0]
            sender1Sync.noArgsReturnOut[1] -> receiver1.noArgsReturnSync[1]

            sender1Sync.primitiveReturnOut[0] -> receiver1.primitiveReturnSync[0]
            sender1Sync.primitiveReturnOut[1] -> receiver1.primitiveReturnSync[1]

            sender1Sync.stringReturnOut[0] -> receiver1.stringReturnSync[0]
            sender1Sync.stringReturnOut[1] -> receiver1.stringReturnSync[1]

            sender1Sync.stringAliasReturnOut[0] -> receiver1.stringAliasReturnSync[0]
            sender1Sync.stringAliasReturnOut[1] -> receiver1.stringAliasReturnSync[1]

            sender1Sync.enumReturnOut[0] -> receiver1.enumReturnSync[0]
            sender1Sync.enumReturnOut[1] -> receiver1.enumReturnSync[1]

            sender1Sync.arrayReturnOut[0] -> receiver1.arrayReturnSync[0]
            sender1Sync.arrayReturnOut[1] -> receiver1.arrayReturnSync[1]

            sender1Sync.arrayStringAliasReturnOut[0] -> receiver1.arrayStringAliasReturnSync[0]
            sender1Sync.arrayStringAliasReturnOut[1] -> receiver1.arrayStringAliasReturnSync[1]

            sender1Sync.structReturnOut[0] -> receiver1.structReturnSync[0]
            sender1Sync.structReturnOut[1] -> receiver1.structReturnSync[1]
        }

        connections SyncInstance2 {
            sender2Sync.noArgsOut[0] -> receiver2.noArgsSync[0]
            sender2Sync.noArgsOut[1] -> receiver2.noArgsSync[1]

            sender2Sync.primitiveArgsOut[0] -> receiver2.primitiveArgsSync[0]
            sender2Sync.primitiveArgsOut[1] -> receiver2.primitiveArgsSync[1]

            sender2Sync.stringArgsOut[0] -> receiver2.stringArgsSync[0]
            sender2Sync.stringArgsOut[1] -> receiver2.stringArgsSync[1]

            sender2Sync.enumArgsOut[0] -> receiver2.enumArgsSync[0]
            sender2Sync.enumArgsOut[1] -> receiver2.enumArgsSync[1]

            sender2Sync.arrayArgsOut[0] -> receiver2.arrayArgsSync[0]
            sender2Sync.arrayArgsOut[1] -> receiver2.arrayArgsSync[1]

            sender2Sync.structArgsOut[0] -> receiver2.structArgsSync[0]
            sender2Sync.structArgsOut[1] -> receiver2.structArgsSync[1]

            sender2Sync.noArgsReturnOut[0] -> receiver2.noArgsReturnSync[0]
            sender2Sync.noArgsReturnOut[1] -> receiver2.noArgsReturnSync[1]

            sender2Sync.primitiveReturnOut[0] -> receiver2.primitiveReturnSync[0]
            sender2Sync.primitiveReturnOut[1] -> receiver2.primitiveReturnSync[1]

            sender2Sync.stringReturnOut[0] -> receiver2.stringReturnSync[0]
            sender2Sync.stringReturnOut[1] -> receiver2.stringReturnSync[1]

            sender2Sync.stringAliasReturnOut[0] -> receiver2.stringAliasReturnSync[0]
            sender2Sync.stringAliasReturnOut[1] -> receiver2.stringAliasReturnSync[1]

            sender2Sync.enumReturnOut[0] -> receiver2.enumReturnSync[0]
            sender2Sync.enumReturnOut[1] -> receiver2.enumReturnSync[1]

            sender2Sync.arrayReturnOut[0] -> receiver2.arrayReturnSync[0]
            sender2Sync.arrayReturnOut[1] -> receiver2.arrayReturnSync[1]

            sender2Sync.arrayStringAliasReturnOut[0] -> receiver2.arrayStringAliasReturnSync[0]
            sender2Sync.arrayStringAliasReturnOut[1] -> receiver2.arrayStringAliasReturnSync[1]

            sender2Sync.structReturnOut[0] -> receiver2.structReturnSync[0]
            sender2Sync.structReturnOut[1] -> receiver2.structReturnSync[1]
        }

        connections SyncSerialInstance1 {
            sender1Sync.serialOut[TestDeploymentPort.NO_ARGS_SYNC] -> receiver1.noArgsSync[2]
            sender1Sync.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_SYNC] -> receiver1.primitiveArgsSync[2]
            sender1Sync.serialOut[TestDeploymentPort.STRING_ARGS_SYNC] -> receiver1.stringArgsSync[2]
            sender1Sync.serialOut[TestDeploymentPort.ENUM_ARGS_SYNC] -> receiver1.enumArgsSync[2]
            sender1Sync.serialOut[TestDeploymentPort.ARRAY_ARGS_SYNC] -> receiver1.arrayArgsSync[2]
            sender1Sync.serialOut[TestDeploymentPort.STRUCT_ARGS_SYNC] -> receiver1.structArgsSync[2]

            sender1Sync.noArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.NO_ARGS_SYNC]
            sender1Sync.primitiveArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]
            sender1Sync.stringArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRING_ARGS_SYNC]
            sender1Sync.enumArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ENUM_ARGS_SYNC]
            sender1Sync.arrayArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.ARRAY_ARGS_SYNC]
            sender1Sync.structArgsOut[2] -> receiver1.serialIn[TestDeploymentPort.STRUCT_ARGS_SYNC]
        }

        connections SyncSerialInstance2 {
            sender2Sync.serialOut[TestDeploymentPort.NO_ARGS_SYNC] -> receiver2.noArgsSync[2]
            sender2Sync.serialOut[TestDeploymentPort.PRIMITIVE_ARGS_SYNC] -> receiver2.primitiveArgsSync[2]
            sender2Sync.serialOut[TestDeploymentPort.STRING_ARGS_SYNC] -> receiver2.stringArgsSync[2]
            sender2Sync.serialOut[TestDeploymentPort.ENUM_ARGS_SYNC] -> receiver2.enumArgsSync[2]
            sender2Sync.serialOut[TestDeploymentPort.ARRAY_ARGS_SYNC] -> receiver2.arrayArgsSync[2]
            sender2Sync.serialOut[TestDeploymentPort.STRUCT_ARGS_SYNC] -> receiver2.structArgsSync[2]

            sender2Sync.noArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.NO_ARGS_SYNC]
            sender2Sync.primitiveArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.PRIMITIVE_ARGS_SYNC]
            sender2Sync.stringArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRING_ARGS_SYNC]
            sender2Sync.enumArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ENUM_ARGS_SYNC]
            sender2Sync.arrayArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.ARRAY_ARGS_SYNC]
            sender2Sync.structArgsOut[2] -> receiver2.serialIn[TestDeploymentPort.STRUCT_ARGS_SYNC]
        }
    }

}