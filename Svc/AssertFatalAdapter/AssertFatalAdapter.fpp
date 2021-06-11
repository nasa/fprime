module Svc {

  @ A component for turning FW_ASSERTs into FATALs
  passive component AssertFatalAdapter {

    @ FPP from XML: original path was Svc/AssertFatalAdapter/AssertFatalEvents.xml
    include "AssertFatalEvents.fppi"

    @ Port for emitting events
    event port eventOut

    @ Port for emitting text events
    text event port textEventOut

    @ Port for getting the time
    time get port timeGetOut

  }

}
