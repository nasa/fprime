module Drv {

    passive component StreamCrossover {

        output port streamOut: Drv.ByteStreamSend

        sync input port streamIn: Drv.ByteStreamRecv

        @ Indicates buffer failed to send to streamOut.
        event StreamOutError(sendStatus: Drv.SendStatus) \
            severity warning high \
            format "StreamCrossover StreamOut Error: {}"

        @ Allows for deallocation after bad receive status
        output port errorDeallocate: Fw.BufferSend

        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

    }

}
