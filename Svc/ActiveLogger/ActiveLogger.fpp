module Svc {

  @ A component for logging events
  active component ActiveLogger {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ Severity level for event filtering
    @ Similar to Fw::LogSeverity, but no FATAL event
    enum FilterSeverity {
      WARNING_HI = 0 @< Filter WARNING_HI events
      WARNING_LO = 1 @< Filter WARNING_LO events
      COMMAND = 2 @< Filter COMMAND events
      ACTIVITY_HI = 3 @< Filter ACTIVITY_HI events
      ACTIVITY_LO = 4 @< Filter ACTIVITY_LO events
      DIAGNOSTIC = 5 @< Filter DIAGNOSTIC events
    }

    # TODO: Consider replacing this enum with Fw::Enabled
    # However, the sense of 0 and 1 are reversed
    @ Enabled and disabled state
    enum Enabled {
      ENABLED = 0 @< Enabled state
      DISABLED = 1 @< Disabled state
    }

    # ----------------------------------------------------------------------
    # Internal ports
    # ----------------------------------------------------------------------

    @ Internal interface to send log messages to component thread
    internal port loqQueue(
                            $id: FwEventIdType @< Log ID
                            timeTag: Fw.Time @< Time Tag
                            $severity: Fw.LogSeverity @< The severity argument
                            args: Fw.LogBuffer @< Buffer containing serialized log entry
                          ) \
      priority 1 \
      drop

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Event input port
    sync input port LogRecv: Fw.Log

    @ Packet send port
    output port PktSend: Fw.Com

    @ FATAL event announce port
    output port FatalAnnounce: Svc.FatalEvent

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Port for receiving commands
    command recv port CmdDisp

    @ Port for sending command registration requests
    command reg port CmdReg

    @ Port for sending command responses
    command resp port CmdStatus

    @ Port for emitting events
    event port Log

    @ Port for emitting text events
    text event port LogText

    @ Port for getting the time
    time get port Time

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Set filter for reporting events. Events are not stored in component.
    sync command SET_EVENT_FILTER(
                                   filterLevel: FilterSeverity @< Filter level
                                   filterEnabled: Enabled @< Filter state
                                 ) \
      opcode 0

    @ Filter a particular ID
    async command SET_ID_FILTER(
                                 ID: U32
                                 idFilterEnabled: Enabled @< ID filter state
                               ) \
      opcode 2

    @ Dump the filter states via events
    async command DUMP_FILTER_STATE \
      opcode 3

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Dump severity filter state
    event SEVERITY_FILTER_STATE(
                                 $severity: FilterSeverity @< The severity level
                                 enabled: bool
                               ) \
      severity activity low \
      id 0 \
      format "{} filter state. {}"

    @ Indicate ID is filtered
    event ID_FILTER_ENABLED(
                             ID: U32 @< The ID filtered
                           ) \
      severity activity high \
      id 1 \
      format "ID {} is filtered."

    @ Attempted to add ID to full ID filter ID
    event ID_FILTER_LIST_FULL(
                               ID: U32 @< The ID filtered
                             ) \
      severity warning low \
      id 2 \
      format "ID filter list is full. Cannot filter {} ."

    @ Removed an ID from the filter
    event ID_FILTER_REMOVED(
                             ID: U32 @< The ID removed
                           ) \
      severity activity high \
      id 3 \
      format "ID filter ID {} removed."

    @ ID not in filter
    event ID_FILTER_NOT_FOUND(
                               ID: U32 @< The ID removed
                             ) \
      severity warning low \
      id 4 \
      format "ID filter ID {} not found."

  }

}
