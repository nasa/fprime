module Svc {

  @ Time cycle Port with timestamp argument
  port Cycle(
              ref cycleStart: Os.RawTime @< Cycle start timestamp
            )

}
