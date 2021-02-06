module Svc {

  type TimerVal

  @ Time cycle Port with timestamp argument
  port Cycle(
              cycleStart: Svc.TimerVal @< Cycle start timer value
            )

}
