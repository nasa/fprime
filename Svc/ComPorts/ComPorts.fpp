module Svc {

    enum ComSendStatus {
        READY = 0 @< Comm is ready to receive buffer
        FAIL = 1 @< Comm failure on last send
    }

    @ Port that indicates status of communication
    port ComStatus(
                    ref ComStatus: ComSendStatus @< Status of communication state
    )
}