module FppTest {

  queued component Sender {

    import TypedPortsOutputs

    internal port done
    sync input port replyIn: Reply
    sync input port serialReplyIn: Reply

    output port serialOut : [TestDeploymentPort.N] serial

  }

}
