module FppTest {

  module SmInstanceJunction {

    queued component Basic {

      include "../../internal/junction/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smJunctionBasic: SmJunction.Basic priority 1 assert

    }

  }

}
