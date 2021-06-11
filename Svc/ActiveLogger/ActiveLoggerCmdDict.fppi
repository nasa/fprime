enum EventLevel {
  WARNING_HI = 0
  WARNING_LO = 1
  COMMAND = 2
  ACTIVITY_HI = 3
  ACTIVITY_LO = 4
  DIAGNOSTIC = 5
}

enum FilterEnabled {
  ENABLED = 0
  DISABLED = 1
}

@ Set filter for reporting events. Events are not stored in component.
sync command SET_EVENT_FILTER(
                               filterLevel: EventLevel @< Filter level
                               filterEnabled: FilterEnabled @< Filter state
                             ) \
  opcode 0

enum IdFilterEnabled {
  ID_ENABLED = 0
  ID_DISABLED = 1
}

@ Filter a particular ID
async command SET_ID_FILTER(
                             ID: U32
                             idFilterEnabled: IdFilterEnabled @< ID filter state
                           ) \
  opcode 2

@ Dump the filter states via events
async command DUMP_FILTER_STATE \
  opcode 3
