module FppTest {

  module SmInstanceState {

    queued component Basic {

      include "../../internal/state/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smStateBasic: SmState.Basic priority 1 assert

    }

  }

}
