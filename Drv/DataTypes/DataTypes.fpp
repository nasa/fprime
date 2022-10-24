module Drv {

  type DataBuffer

  @ Data Buffer Port
  port DataBuffer(
                   ref buff: Drv.DataBuffer @< A data buffer
                 )

}
