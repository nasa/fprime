enum EventFilterState {
  WARNING_HI = 0
  WARNING_LO = 1
  COMMAND = 2
  ACTIVITY_HI = 3
  ACTIVITY_LO = 4
  DIAGNOSTIC = 5
}

@ Dump severity filter state
event SEVERITY_FILTER_STATE(
                             $severity: eventFilterState @< The severity level
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
