module Drv {

  port SerialWrite(
                    ref serBuffer: Fw.Buffer
                  )

}

module Drv {

  enum SerialReadStatus {
    SER_OK = 0 @< Transaction okay
    SER_PARITY_ERR = 1 @< Parity error on data
    SER_NO_BUFFERS = 2 @< Serial driver ran out of buffers
    SER_BUFFER_TOO_SMALL = 3 @< Target buffer is too small
    SER_OTHER_ERR = 4 @< Other errors that don't fit
  }

  port SerialRead(
                   ref serBuffer: Fw.Buffer @< Buffer containing data
                   ref status: SerialReadStatus @< Status of read
                 )

}
