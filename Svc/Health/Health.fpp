module Svc {

  @ A component for checking the health of active components
  queued component Health {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Ping output port
    output port PingSend: [HealthPingPorts] Svc.Ping

    @ Ping return port
    async input port PingReturn: [HealthPingPorts] Svc.Ping

    @ Run port
    sync input port Run: Svc.Sched

    @ Run port
    output port WdogStroke: Svc.WatchDog

    # ----------------------------------------------------------------------
    # Port matching specifiers
    # ----------------------------------------------------------------------

    match PingSend with PingReturn

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------


    @ A command to enable or disable health checks
    async command HLTH_ENABLE(
                               enable: Fw.Enabled @< whether or not health checks are enabled
                             ) \
      opcode 0x0

    @ Ignore a particular ping entry
    async command HLTH_PING_ENABLE(
                                    $entry: string size 40 @< The entry to enable/disable
                                    enable: Fw.Enabled @< whether or not a port is pinged
                                  ) \
      opcode 0x1

    @ Change ping value
    async command HLTH_CHNG_PING(
                                  $entry: string size 40 @< The entry to modify
                                  warningValue: U32 @< Ping warning threshold
                                  fatalValue: U32 @< Ping fatal threshold
                                ) \
      opcode 0x2

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Warn that a ping target is longer than the warning value
    event HLTH_PING_WARN(
                          $entry: string size 40 @< The entry passing the warning level
                        ) \
      severity warning high \
      id 0x0 \
      format "Ping entry {} late warning"

    @ Declare FATAL since task is no longer responding
    event HLTH_PING_LATE(
                          $entry: string size 40 @< The entry passing the warning level
                        ) \
      severity fatal \
      id 0x1 \
      format "Ping entry {} did not respond"

    @ Declare FATAL since task is no longer responding
    event HLTH_PING_WRONG_KEY(
                               $entry: string size 40 @< The entry passing the warning level
                               badKey: U32 @< The incorrect key value
                             ) \
      severity fatal \
      id 0x2 \
      format "Ping entry {} responded with wrong key 0x{x}"

    @ Report checking turned on or off
    event HLTH_CHECK_ENABLE(
                             enabled: Fw.Enabled @< If health checking is enabled
                           ) \
      severity activity high \
      id 0x3 \
      format "Health checking set to {}"

    @ Report a particular entry on or off
    event HLTH_CHECK_PING(
                           enabled: Fw.Enabled @< If health pinging is enabled for a particular entry
                           $entry: string size 40 @< The entry passing the warning level
                         ) \
      severity activity high \
      id 0x4 \
      format "Health checking set to {} for {}"

    @ Entry was not found
    event HLTH_CHECK_LOOKUP_ERROR(
                                   $entry: string size 40 @< The entry passing the warning level
                                 ) \
      severity warning low \
      id 0x5 \
      format "Couldn't find entry {}"

    @ Report changed ping
    event HLTH_PING_UPDATED(
                             $entry: string size 40 @< The entry changed
                             warn: U32 @< The new warning value
                             $fatal: U32 @< The new FATAL value
                           ) \
      severity activity high \
      id 0x6 \
      format "Health ping for {} changed to WARN {} FATAL {}"

    @ Report changed ping
    event HLTH_PING_INVALID_VALUES(
                                    $entry: string size 40 @< The entry changed
                                    warn: U32 @< The new warning value
                                    $fatal: U32 @< The new FATAL value
                                  ) \
      severity warning high \
      id 0x7 \
      format "Health ping for {} invalid values: WARN {} FATAL {}"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of overrun warnings
    telemetry PingLateWarnings: U32 id 0x0

  }

}
