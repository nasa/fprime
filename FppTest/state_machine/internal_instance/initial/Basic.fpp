module FppTest {

  module SmInstanceInitial {

    queued component Basic {

      include "../../internal/initial/include/Basic.fppi"

      state machine instance basic: Basic

      state machine instance smInitialBasic: SmInitial.Basic

    }

  }

}
