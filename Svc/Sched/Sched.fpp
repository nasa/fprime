module Svc {

  @ Scheduler Port with order argument
  port Sched(
              context: NATIVE_UINT_TYPE @< The call order
            )

}
