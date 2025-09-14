module Drv {

    @ Status returned by the send call
    enum ByteStreamStatus {
        OP_OK         @< Operation worked as expected
        SEND_RETRY    @< Data send should be retried
        RECV_NO_DATA  @< Receive worked, but there was no data 
        OTHER_ERROR   @< Error occurred, retrying may succeed
    }

    @ Port to exchange buffer and status with the ByteStreamDriver model
    @ This port is used for receiving data from the driver as well as on
    @ callback of an asynchronous send call
    port ByteStreamData(
        ref buffer: Fw.Buffer,
        status: ByteStreamStatus
    )

    @ Synchronous only - Send data out through the byte stream
    port ByteStreamSend(
        ref sendBuffer: Fw.Buffer @< Data to send
    ) -> ByteStreamStatus

    @ Signal indicating the driver is ready to send and received data
    port ByteStreamReady()

}
