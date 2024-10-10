module Drv {

  passive component LinuxGpioDriver {
    include "../Interfaces/GpioInterface.fppi"


    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    event port Log

    text event port LogText

    time get port Time

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------
    event OpenChip(chip: string, label: string, pins: U32) severity diagnostic format "Opened {}[{}] with {} lines"

    event OpenChipError(chip: string, status: Os.FileStatus) severity warning high format "Failed to open chip {}: {}"

    event OpenPinError(chip: string, pin: U32, status: Os.FileStatus) severity warning high format "Failed to open pin {}.{}: {}"
  }

}
