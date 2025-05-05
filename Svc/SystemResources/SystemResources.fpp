module Svc {

  enum SystemResourceEnabled {
    DISABLED = 0
    ENABLED = 1
  }

  passive component SystemResources {

    @ Run port
    guarded input port run: [1] Svc.Sched
    
    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Time get port
    time get port Time

    @ Command registration port
    command reg port CmdReg

    @ Command received port
    command recv port CmdDisp

    @ Command response port
    command resp port CmdStatus

    @ Text event port
    text event port LogText

    @ Event port
    event port Log

    @ Telemetry port
    telemetry port Tlm


    @ A command to enable or disable system resource telemetry
    guarded command ENABLE(
                            enable: SystemResourceEnabled @< whether or not system resource telemetry is enabled
                          ) \
      opcode 0

    @ Total system memory in KB
    telemetry MEMORY_TOTAL: U64 id 0 \
      format "{} KB"

    @ System memory used in KB
    telemetry MEMORY_USED: U64 id 1 \
      format "{} KB"

    @ System non-volatile available in KB
    telemetry NON_VOLATILE_TOTAL: U64 id 2 \
      format "{} KB"

    @ System non-volatile available in KB
    telemetry NON_VOLATILE_FREE: U64 id 3 \
      format "{} KB"

    @ System's CPU Percentage
    telemetry CPU: F32 id 4 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_00: F32 id 5 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_01: F32 id 6 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_02: F32 id 7 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_03: F32 id 8 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_04: F32 id 9 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_05: F32 id 10 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_06: F32 id 11 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_07: F32 id 12 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_08: F32 id 13 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_09: F32 id 14 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_10: F32 id 15 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_11: F32 id 16 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_12: F32 id 17 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_13: F32 id 18 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_14: F32 id 19 format "{.2f} percent"

    @ System's CPU Percentage
    telemetry CPU_15: F32 id 20 format "{.2f} percent"

  }

}
