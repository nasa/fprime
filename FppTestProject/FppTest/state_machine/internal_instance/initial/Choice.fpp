module FppTest {

  module SmInstanceInitial {

    active component Choice {

      include "../../internal/initial/include/Choice.fppi"

      state machine instance $choice: Choice priority 1 assert

      state machine instance smInitialChoice: SmInitial.Choice priority 2 block

    }

  }

}
