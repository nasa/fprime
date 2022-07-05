# module Srv {
#
#    @ Port that returns buffer
#    port CommWrite(
#                     ref comBuffer: Fw.Buffer
#                   )
#}

module Srv {

    enum ComReadStatus {
        COM_SUCCESS = 0 @< Queue is ready to receive buff
        COM_FAIL = 1 @< Queue is full
    }

    @ Ports that store and read buffer
    port ComRead(
                    ref comBuffer: Fw.Buffer @< Buffer that contains data
                    ref comStatus: ComReadStatus @< Status of queue
                )
}