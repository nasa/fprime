module Drv {

  port GpioWrite(
                  state: bool
                )

}

module Drv {

  port GpioRead(
                 ref state: bool
               )

}
