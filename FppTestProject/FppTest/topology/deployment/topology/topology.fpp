module FppTest {

  topology FppTest {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance a1
    instance a2
    instance b1
    instance b2

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Instance1 {
      a1.dataOut -> b1.dataIn
      b1.dataOut -> a1.dataIn
    }

    connections Instance2 {
      a2.dataOut -> b2.dataIn
      b2.dataOut -> a2.dataIn
    }

  }

}
