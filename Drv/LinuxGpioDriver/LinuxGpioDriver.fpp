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
    event OpenChip(chip: string, chipLabel: string, pin: U32, pinMessage: string) severity diagnostic format "Opened GPIO chip {}[{}] pin {}[{}]"

    event OpenChipError(chip: string, status: Os.FileStatus) severity warning high format "Failed to open GPIO chip {}: {}"

    event OpenPinError(chip: string, pin: U32, pinMessage: string, status: Os.FileStatus) severity warning high format "Failed to open GPIO chip {} pin {} [{}]: {}"

    event InterruptReadError(expected: U32, got: U32) severity warning high format "Interrupt data read expected {} byes and got {}"

    event PollingError(error_number: I32) severity warning high format "Interrupt polling returned errno: {}"

  }

}
