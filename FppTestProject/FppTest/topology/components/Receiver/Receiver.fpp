module FppTest {

  active component Receiver {

    import TypedPortsSyncInputs
    import TypedPortsGuardedInputs
    import TypedPortsAsync

    output port replyOut: [SenderId.N] Reply

    async input port serialIn : [TestDeploymentPort.N] serial

  }

}
