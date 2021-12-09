module Drv {

  passive component LinuxGpioDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    sync input port gpioWrite: Drv.GpioWrite

    output port intOut: [2] Svc.Cycle

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    event port Log

    text event port LogText

    sync input port gpioRead: Drv.GpioRead

    time get port Time

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }

}
