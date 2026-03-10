module FppTest {

  topology Receiver {
    instance receiver1
    instance receiver2
  }

  topology FppTest {

    instance Sync
    instance Guarded
    instance Async
    instance TopPorts

  }

}
