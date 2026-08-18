module FppTest {
    interface TypedPortsAsync {
        async input port noArgsAsync: [3] NoArgs

        async input port primitiveArgsAsync: [3] PrimitiveArgs

        async input port stringArgsAsync: [3] StringArgs

        async input port enumArgsAsync: [3] EnumArgs assert

        async input port arrayArgsAsync: [3] ArrayArgs priority 10 block

        async input port structArgsAsync: [3] StructArgs priority 5 drop

        async input port enumArgsHook: [3] EnumArgs hook

    }
}
