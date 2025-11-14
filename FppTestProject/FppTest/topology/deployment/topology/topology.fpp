module FppTest {

  topology FppTest {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance sender1
    instance sender2
    instance receiver1
    instance receiver2

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Instance1Sync {
      sender1.noArgsOut[0] -> receiver1.noArgsSync[0]
      sender1.noArgsOut[1] -> receiver1.noArgsSync[1]

      sender1.primitiveArgsOut[0] -> receiver1.primitiveArgsSync[0]
      sender1.primitiveArgsOut[1] -> receiver1.primitiveArgsSync[1]

      sender1.stringArgsOut[0] -> receiver1.stringArgsSync[0]
      sender1.stringArgsOut[1] -> receiver1.stringArgsSync[1]

      sender1.enumArgsOut[0] -> receiver1.enumArgsSync[0]
      sender1.enumArgsOut[1] -> receiver1.enumArgsSync[1]

      sender1.arrayArgsOut[0] -> receiver1.arrayArgsSync[0]
      sender1.arrayArgsOut[1] -> receiver1.arrayArgsSync[1]

      sender1.structArgsOut[0] -> receiver1.structArgsSync[0]
      sender1.structArgsOut[1] -> receiver1.structArgsSync[1]

      sender1.noArgsReturnOut[0] -> receiver1.noArgsReturnSync[0]
      sender1.noArgsReturnOut[1] -> receiver1.noArgsReturnSync[1]

      sender1.primitiveReturnOut[0] -> receiver1.primitiveReturnSync[0]
      sender1.primitiveReturnOut[1] -> receiver1.primitiveReturnSync[1]

      sender1.stringReturnOut[0] -> receiver1.stringReturnSync[0]
      sender1.stringReturnOut[1] -> receiver1.stringReturnSync[1]

      sender1.stringAliasReturnOut[0] -> receiver1.stringAliasReturnSync[0]
      sender1.stringAliasReturnOut[1] -> receiver1.stringAliasReturnSync[1]

      sender1.enumReturnOut[0] -> receiver1.enumReturnSync[0]
      sender1.enumReturnOut[1] -> receiver1.enumReturnSync[1]

      sender1.arrayReturnOut[0] -> receiver1.arrayReturnSync[0]
      sender1.arrayReturnOut[1] -> receiver1.arrayReturnSync[1]

      sender1.arrayStringAliasReturnOut[0] -> receiver1.arrayStringAliasReturnSync[0]
      sender1.arrayStringAliasReturnOut[1] -> receiver1.arrayStringAliasReturnSync[1]

      sender1.structReturnOut[0] -> receiver1.structReturnSync[0]
      sender1.structReturnOut[1] -> receiver1.structReturnSync[1]

      sender1.enumArgsOut[0] -> receiver1.enumArgsSync[0]
      sender1.enumArgsOut[1] -> receiver1.enumArgsSync[1]

      sender1.enumArgsOut[0] -> receiver1.enumArgsSync[0]
      sender1.enumArgsOut[1] -> receiver1.enumArgsSync[1]

      sender1.enumArgsOut[0] -> receiver1.enumArgsSync[0]
      sender1.enumArgsOut[1] -> receiver1.enumArgsSync[1]
    }

    connections Instance2Sync {
      sender2.noArgsOut[0] -> receiver2.noArgsSync[0]
      sender2.noArgsOut[1] -> receiver2.noArgsSync[1]

      sender2.primitiveArgsOut[0] -> receiver2.primitiveArgsSync[0]
      sender2.primitiveArgsOut[1] -> receiver2.primitiveArgsSync[1]

      sender2.stringArgsOut[0] -> receiver2.stringArgsSync[0]
      sender2.stringArgsOut[1] -> receiver2.stringArgsSync[1]

      sender2.enumArgsOut[0] -> receiver2.enumArgsSync[0]
      sender2.enumArgsOut[1] -> receiver2.enumArgsSync[1]

      sender2.arrayArgsOut[0] -> receiver2.arrayArgsSync[0]
      sender2.arrayArgsOut[1] -> receiver2.arrayArgsSync[1]

      sender2.structArgsOut[0] -> receiver2.structArgsSync[0]
      sender2.structArgsOut[1] -> receiver2.structArgsSync[1]

      sender2.noArgsReturnOut[0] -> receiver2.noArgsReturnSync[0]
      sender2.noArgsReturnOut[1] -> receiver2.noArgsReturnSync[1]

      sender2.primitiveReturnOut[0] -> receiver2.primitiveReturnSync[0]
      sender2.primitiveReturnOut[1] -> receiver2.primitiveReturnSync[1]

      sender2.stringReturnOut[0] -> receiver2.stringReturnSync[0]
      sender2.stringReturnOut[1] -> receiver2.stringReturnSync[1]

      sender2.stringAliasReturnOut[0] -> receiver2.stringAliasReturnSync[0]
      sender2.stringAliasReturnOut[1] -> receiver2.stringAliasReturnSync[1]

      sender2.enumReturnOut[0] -> receiver2.enumReturnSync[0]
      sender2.enumReturnOut[1] -> receiver2.enumReturnSync[1]

      sender2.arrayReturnOut[0] -> receiver2.arrayReturnSync[0]
      sender2.arrayReturnOut[1] -> receiver2.arrayReturnSync[1]

      sender2.arrayStringAliasReturnOut[0] -> receiver2.arrayStringAliasReturnSync[0]
      sender2.arrayStringAliasReturnOut[1] -> receiver2.arrayStringAliasReturnSync[1]

      sender2.structReturnOut[0] -> receiver2.structReturnSync[0]
      sender2.structReturnOut[1] -> receiver2.structReturnSync[1]

      sender2.enumArgsOut[0] -> receiver2.enumArgsSync[0]
      sender2.enumArgsOut[1] -> receiver2.enumArgsSync[1]

      sender2.enumArgsOut[0] -> receiver2.enumArgsSync[0]
      sender2.enumArgsOut[1] -> receiver2.enumArgsSync[1]

      sender2.enumArgsOut[0] -> receiver2.enumArgsSync[0]
      sender2.enumArgsOut[1] -> receiver2.enumArgsSync[1]
    }

  }

}
