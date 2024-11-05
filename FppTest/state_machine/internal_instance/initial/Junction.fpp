module FppTest {

  module SmInstanceInitial {

    active component Junction {

      include "../../internal/initial/include/Choice.fppi"

      state machine instance $choice: Junction priority 1 assert

      state machine instance smInitialJunction: SmInitial.Junction priority 2 block

    }

  }

}
