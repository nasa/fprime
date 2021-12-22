module Drv {

  enum SendStatus {
    SEND_OK = 0 @< Send worked as expected
    SEND_RETRY = 1 @< Data send should be retried
    SEND_ERROR = 2 @< Send error occurred retrying may succeed
  }

  port ByteStreamSend(
                       ref sendBuffer: Fw.Buffer
                     ) -> SendStatus


  enum RecvStatus {
    RECV_OK = 0 @< Receive worked as expected
    RECV_ERROR = 1 @< Receive error occurred retrying may succeed
  }

  port ByteStreamRecv(
                       ref recvBuffer: Fw.Buffer
                       recvStatus: RecvStatus
                     )


  enum PollStatus {
    POLL_OK = 0 @< Poll successfully received data
    POLL_RETRY = 1 @< No data available, retry later
    POLL_ERROR = 2 @< Error received when polling
  }

  port ByteStreamPoll(
                       ref pollBuffer: Fw.Buffer
                     ) -> PollStatus

  port ByteStreamReady()

  passive component ByteStreamDriverModel {

    output port ready: Drv.ByteStreamReady

    output port $recv: Drv.ByteStreamRecv

    guarded input port send: Drv.ByteStreamSend

    guarded input port poll: Drv.ByteStreamPoll

    output port allocate: Fw.BufferGet

    output port deallocate: Fw.BufferSend

  }

}
