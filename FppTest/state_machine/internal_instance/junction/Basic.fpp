module FppTest {

  module SmInstanceJunction {

    queued component Basic {

      sync input port schedIn: Svc.Sched

      include "../../internal/choice/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smJunctionBasic: SmJunction.Basic priority 1 assert

    }

  }

}
