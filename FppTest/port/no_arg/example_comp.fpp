module ExampleComponents {

  @ An example component
  active component Example {

    sync input port noArgPort: ExampleComponents.NoArgs

    async input port asyncNoArgPort: ExampleComponents.NoArgs

    guarded input port guardedNoArgPort: ExampleComponents.NoArgs

    @ Example input port
    sync input port exampleInput: APortCollection.Some

  }

}
