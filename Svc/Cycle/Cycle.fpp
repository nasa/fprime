module Svc {

  # type TimerVal
  # Note: AH! SERIALIZED_SIZE in CyclePort cannot build properly with the CMake linking shenanigans
  @ Time cycle Port with timestamp argument
  port Cycle(
              ref cycleStart: Os.RawTime @< Cycle start timestamp
            )

}
