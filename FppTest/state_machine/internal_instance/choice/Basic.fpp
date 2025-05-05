module FppTest {

  module SmInstanceChoice {

    queued component Basic {

      sync input port schedIn: Svc.Sched

      include "../../internal/choice/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smChoiceBasic: SmChoice.Basic priority 1 assert

    }

  }

}
