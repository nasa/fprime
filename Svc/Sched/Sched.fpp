module Svc {

  @ Scheduler Port with order argument
  port Sched(
              context: U32 @< The call order
            )

}
