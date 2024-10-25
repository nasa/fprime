module FppTest {

  module SmInstanceState {

    queued component Basic {

      sync input port schedIn: Svc.Sched

      include "../../internal/state/include/Basic.fppi"

      state machine instance basic1: Basic

      state machine instance basic2: Basic

      state machine instance smStateBasic1: SmState.Basic priority 1 assert

      state machine instance smStateBasic2: SmState.Basic priority 2 drop

    }

  }

}
