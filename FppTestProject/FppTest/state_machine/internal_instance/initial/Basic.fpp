module FppTest {

  module SmInstanceInitial {

    queued component Basic {

      sync input port schedIn: Svc.Sched

      include "../../internal/initial/include/Basic.fppi"

      state machine instance basic1: Basic

      state machine instance basic2: Basic

      state machine instance smInitialBasic1: SmInitial.Basic

      state machine instance smInitialBasic2: SmInitial.Basic

    }

  }

}
