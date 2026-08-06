register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/EventSeverityFilter.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/EventSeverityFilter.hpp"
  DEPENDS
    Fw_Log
)

### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/EventSeverityFilterTestMain.cpp"
  DEPENDS
    Svc_Types_EventSeverityFilter
)
