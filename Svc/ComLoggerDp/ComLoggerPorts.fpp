module Svc {

  @ Port for starting Com data product recording
  @ Config encoded as: (packetsPerContainer << 16) | priority
  port ComLoggerStart(
    config: U32 @< Encoded configuration
  )

  @ Port for stopping Com data product recording
  port ComLoggerStop()

}
