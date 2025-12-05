module FppTest {

  topology Receiver {
    instance receiver1
    instance receiver2
  }

  topology FppTest {

    import Sync
    import Guarded
    import Async
  }

}
