module FppTest {

  module SmInstanceInitial {

    queued component Basic {

      sync input port schedIn: Svc.Sched

      include "../../internal/initial/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smInitialBasic: SmInitial.Basic

    }

  }

}
