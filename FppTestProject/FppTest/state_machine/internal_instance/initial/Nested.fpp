module FppTest {

  module SmInstanceInitial {

    active component Nested {

      include "../../internal/initial/include/Nested.fppi"

      state machine instance nested: Nested priority 3 drop

      state machine instance smInitialNested: SmInitial.Nested priority 4 hook

    }

  }

}
