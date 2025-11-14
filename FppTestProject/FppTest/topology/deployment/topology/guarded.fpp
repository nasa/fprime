module FppTest {

    topology Guarded {
        import Receiver

        instance sender1Guarded
        instance sender2Guarded

        connections Instance1 {
            sender1Guarded.noArgsOut[0] -> receiver1.noArgsSync[0]
            sender1Guarded.noArgsOut[1] -> receiver1.noArgsSync[1]

            sender1Guarded.primitiveArgsOut[0] -> receiver1.primitiveArgsSync[0]
            sender1Guarded.primitiveArgsOut[1] -> receiver1.primitiveArgsSync[1]

            sender1Guarded.stringArgsOut[0] -> receiver1.stringArgsSync[0]
            sender1Guarded.stringArgsOut[1] -> receiver1.stringArgsSync[1]

            sender1Guarded.enumArgsOut[0] -> receiver1.enumArgsSync[0]
            sender1Guarded.enumArgsOut[1] -> receiver1.enumArgsSync[1]

            sender1Guarded.arrayArgsOut[0] -> receiver1.arrayArgsSync[0]
            sender1Guarded.arrayArgsOut[1] -> receiver1.arrayArgsSync[1]

            sender1Guarded.structArgsOut[0] -> receiver1.structArgsSync[0]
            sender1Guarded.structArgsOut[1] -> receiver1.structArgsSync[1]

            sender1Guarded.noArgsReturnOut[0] -> receiver1.noArgsReturnSync[0]
            sender1Guarded.noArgsReturnOut[1] -> receiver1.noArgsReturnSync[1]

            sender1Guarded.primitiveReturnOut[0] -> receiver1.primitiveReturnSync[0]
            sender1Guarded.primitiveReturnOut[1] -> receiver1.primitiveReturnSync[1]

            sender1Guarded.stringReturnOut[0] -> receiver1.stringReturnSync[0]
            sender1Guarded.stringReturnOut[1] -> receiver1.stringReturnSync[1]

            sender1Guarded.stringAliasReturnOut[0] -> receiver1.stringAliasReturnSync[0]
            sender1Guarded.stringAliasReturnOut[1] -> receiver1.stringAliasReturnSync[1]

            sender1Guarded.enumReturnOut[0] -> receiver1.enumReturnSync[0]
            sender1Guarded.enumReturnOut[1] -> receiver1.enumReturnSync[1]

            sender1Guarded.arrayReturnOut[0] -> receiver1.arrayReturnSync[0]
            sender1Guarded.arrayReturnOut[1] -> receiver1.arrayReturnSync[1]

            sender1Guarded.arrayStringAliasReturnOut[0] -> receiver1.arrayStringAliasReturnSync[0]
            sender1Guarded.arrayStringAliasReturnOut[1] -> receiver1.arrayStringAliasReturnSync[1]

            sender1Guarded.structReturnOut[0] -> receiver1.structReturnSync[0]
            sender1Guarded.structReturnOut[1] -> receiver1.structReturnSync[1]
        }

        connections Instance2 {
            sender2Guarded.noArgsOut[0] -> receiver2.noArgsSync[0]
            sender2Guarded.noArgsOut[1] -> receiver2.noArgsSync[1]

            sender2Guarded.primitiveArgsOut[0] -> receiver2.primitiveArgsSync[0]
            sender2Guarded.primitiveArgsOut[1] -> receiver2.primitiveArgsSync[1]

            sender2Guarded.stringArgsOut[0] -> receiver2.stringArgsSync[0]
            sender2Guarded.stringArgsOut[1] -> receiver2.stringArgsSync[1]

            sender2Guarded.enumArgsOut[0] -> receiver2.enumArgsSync[0]
            sender2Guarded.enumArgsOut[1] -> receiver2.enumArgsSync[1]

            sender2Guarded.arrayArgsOut[0] -> receiver2.arrayArgsSync[0]
            sender2Guarded.arrayArgsOut[1] -> receiver2.arrayArgsSync[1]

            sender2Guarded.structArgsOut[0] -> receiver2.structArgsSync[0]
            sender2Guarded.structArgsOut[1] -> receiver2.structArgsSync[1]

            sender2Guarded.noArgsReturnOut[0] -> receiver2.noArgsReturnSync[0]
            sender2Guarded.noArgsReturnOut[1] -> receiver2.noArgsReturnSync[1]

            sender2Guarded.primitiveReturnOut[0] -> receiver2.primitiveReturnSync[0]
            sender2Guarded.primitiveReturnOut[1] -> receiver2.primitiveReturnSync[1]

            sender2Guarded.stringReturnOut[0] -> receiver2.stringReturnSync[0]
            sender2Guarded.stringReturnOut[1] -> receiver2.stringReturnSync[1]

            sender2Guarded.stringAliasReturnOut[0] -> receiver2.stringAliasReturnSync[0]
            sender2Guarded.stringAliasReturnOut[1] -> receiver2.stringAliasReturnSync[1]

            sender2Guarded.enumReturnOut[0] -> receiver2.enumReturnSync[0]
            sender2Guarded.enumReturnOut[1] -> receiver2.enumReturnSync[1]

            sender2Guarded.arrayReturnOut[0] -> receiver2.arrayReturnSync[0]
            sender2Guarded.arrayReturnOut[1] -> receiver2.arrayReturnSync[1]

            sender2Guarded.arrayStringAliasReturnOut[0] -> receiver2.arrayStringAliasReturnSync[0]
            sender2Guarded.arrayStringAliasReturnOut[1] -> receiver2.arrayStringAliasReturnSync[1]

            sender2Guarded.structReturnOut[0] -> receiver2.structReturnSync[0]
            sender2Guarded.structReturnOut[1] -> receiver2.structReturnSync[1]
        }
    }

}