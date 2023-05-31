module Drv {

    passive component StreamCrossover {

        output port streamOut: Drv.ByteStreamSend

        sync input port streamIn: Drv.ByteStreamRecv

    }

}
