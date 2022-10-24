module Svc {

  @ A component for turning FW_ASSERTs into FATALs
  passive component AssertFatalAdapter {

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "AssertFatalEvents.fppi"

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Port for emitting events
    event port Log

    @ Port for emitting text events
    text event port LogText

    @ Port for getting the time
    time get port Time

  }

}
