module Drv {
  enum GpioStatus {
    OP_OK @< Operation succeeded
    NOT_OPENED @< Pin was never opened
    INVALID_MODE @< Operation not permitted with current configuration
    UNKNOWN_ERROR @< An unknown error occurred
  }
}

module Drv {

  port GpioWrite(
                  $state: Fw.Logic
                ) -> GpioStatus

}

module Drv {

  port GpioRead(
                 ref $state: Fw.Logic
               ) -> GpioStatus

}
