module Drv {

  port GpioWrite(
                  state: Fw.Logic
                )

}

module Drv {

  port GpioRead(
                 ref state: Fw.Logic
               )

}
