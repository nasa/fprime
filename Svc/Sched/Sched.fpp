module Svc {

  @ Scheduler Port with order argument
  port Sched(
              context: U16 @< The call order
            )

}
