module Svc {

  type TimerVal

  @ Time cycle Port with timestamp argument
  port Cycle(
              ref cycleStart: Svc.TimerVal @< Cycle start timer value
            )

}
