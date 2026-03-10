module FppTest {

  queued component Sender {

    import TypedPortsOutputs

    internal port done
    sync input port replyIn: Reply

  }

}
