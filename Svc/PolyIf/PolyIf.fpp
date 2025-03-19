module Svc {

  @ An enumeration for measurement status
  enum MeasurementStatus {
    OK = 0 @< Measurement was good
    FAILURE = 1 @< Failure to retrieve measurement
    STALE = 2 @< Measurement is stale
  }

  @ Port for setting and getting PolyType values
  port Poly(
             $entry: PolyDbCfg.PolyDbEntry @< The entry to access
             ref status: MeasurementStatus @< The command response argument
             ref $time: Fw.Time @< The time of the measurement
             ref val: Fw.PolyType @< The value to be passed
           )

}
