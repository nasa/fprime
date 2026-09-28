module Svc {

  @ Port for starting Com data product recording
  port ComLoggerStart(
    @ Number of packets per container
    packetsPerContainer: FwSizeType
    @ Data product priority
    $priority: FwDpPriorityType
  )

  @ Port for stopping Com data product recording
  port ComLoggerStop()

}
