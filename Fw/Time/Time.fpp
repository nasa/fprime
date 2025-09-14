module Fw {
  
  @ Data structure for Time 
  struct TimeValue {
    timeBase: TimeBase  @< basis of time (defined by system)
    timeContext: FwTimeContextStoreType  @< user settable value. Could be reboot count, node, etc
    seconds: U32  @< seconds portion of Time
    useconds: U32  @< microseconds portion of Time
  }

  type Time

  @ Time port
  port Time(
    ref $time: Fw.Time @< Reference to Time object
  )

  @ Data structure for Time Interval
  struct TimeIntervalValue {
    seconds: U32  @< seconds portion of TimeInterval
    useconds: U32  @< microseconds portion of TimeInterval
  }

  type TimeInterval

  @ Time interval port
  port TimeInterval(
    ref timeInterval: Fw.TimeInterval @< Reference to TimeInterval object
  )

}
