module Svc {

  @ Port for starting Com data product recording
  port ComLoggerStart(
    @ Number of packets per container
    packetsPerContainer: U32
    @ Data product priority
    $priority: FwDpPriorityType
  )

  @ Port for stopping Com data product recording
  port ComLoggerStop()

}
