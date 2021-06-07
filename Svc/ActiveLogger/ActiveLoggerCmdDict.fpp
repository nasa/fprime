enum EventLevel {
  FILTER_WARNING_HI = 0
  FILTER_WARNING_LO = 1
  FILTER_COMMAND = 2
  FILTER_ACTIVITY_HI = 3
  FILTER_ACTIVITY_LO = 4
  FILTER_DIAGNOSTIC = 5
}

enum FilterEnabled {
  FILTER_ENABLED = 0
  FILTER_DISABLED = 1
}

@ Set filter for reporting events. Events are not stored in component.
sync command SET_EVENT_FILTER(
                               FilterLevel: EventLevel @< Filter level
                               FilterEnable: FilterEnabled @< Filter state
                             ) \
  opcode 0

enum IdFilterEnabled {
  ID_ENABLED = 0
  ID_DISABLED = 1
}

@ Filter a particular ID
async command SET_ID_FILTER(
                             ID: U32
                             IdFilterEnable: IdFilterEnabled @< ID filter state
                           ) \
  opcode 2

@ Dump the filter states via events
async command DUMP_FILTER_STATE \
  opcode 3
