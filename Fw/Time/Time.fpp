module Fw {

  type Time

  @ Time port
  port Time(
             ref $time: Fw.Time @< Reference to Time object
           )

  type TimeInterval

  @ Time interval port
  port TimeInterval(
      ref timeInterval: Fw.TimeInterval @< Reference to TimeInterval object
           )

}
