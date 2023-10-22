module Drv {

    @ Status returned by the send call
    enum SendStatus {
        SEND_OK = 0 @< Send worked as expected
        SEND_RETRY = 1 @< Data send should be retried
        SEND_ERROR = 2 @< Send error occurred retrying may succeed
    }

    @ Send data out through the byte stream
    port ByteStreamSend(
        ref sendBuffer: Fw.Buffer @< Data to send
    ) -> SendStatus

    @ Status associated with the received data
    enum RecvStatus {
        RECV_OK = 0 @< Receive worked as expected
        RECV_ERROR = 1 @< Receive error occurred retrying may succeed
    }

    @ Carries the received bytes stream driver
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

    @ Signal indicating the driver is ready to send and received data
    port ByteStreamReady()
}
