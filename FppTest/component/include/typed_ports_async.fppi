async input port noArgsAsync: [2] NoArgs

async input port primitiveArgsAsync: [2] PrimitiveArgs

async input port stringArgsAsync: [2] StringArgs

async input port enumArgsAsync: [2] EnumArgs assert

async input port arrayArgsAsync: [2] ArrayArgs priority 10 block

async input port structArgsAsync: [2] StructArgs priority 5 drop

async input port enumArgsHook: [2] EnumArgs hook
