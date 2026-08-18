module FppTest {

  active component Receiver {

    import TypedPortsSyncInputs
    import TypedPortsGuardedInputs
    import TypedPortsAsync

    output port replyOut: [SenderId.N] Reply

    sync input port serialIn : [TestDeploymentPort.N] serial
    output port serialReplyOut: [SenderId.N] Reply

  }

}
