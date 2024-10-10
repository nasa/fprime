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
    event OpenChip(chip: string, label: string, pins: U32) severity activity high format "Opened {}[{}] with {} lines"

    event OpenChipError(chip: string, status: Os.FileStatus) severity warning high format "Failed to open chip {}: {}"
  }

}
