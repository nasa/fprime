module FppTest {
    interface TypedPortsSyncInputs {
        # ----------------------------------------------------------------------
        # Typed input ports with no return type
        # ----------------------------------------------------------------------

        sync input port noArgsSync: [3] NoArgs

        sync input port primitiveArgsSync: [3] PrimitiveArgs

        sync input port stringArgsSync: [3] StringArgs

        sync input port enumArgsSync: [3] EnumArgs

        sync input port arrayArgsSync: [3] ArrayArgs

        sync input port structArgsSync: [3] StructArgs

        # ----------------------------------------------------------------------
        # Typed input ports with return type
        # ----------------------------------------------------------------------

        sync input port noArgsReturnSync: [3] NoArgsReturn

        sync input port primitiveReturnSync: [3] PrimitiveReturn

        sync input port stringReturnSync: [3] StringReturn

        sync input port stringAliasReturnSync: [3] StringAliasReturn

        sync input port enumReturnSync: [3] EnumReturn

        sync input port arrayReturnSync: [3] ArrayReturn

        sync input port arrayStringAliasReturnSync: [3] ArrayStringAliasReturn

        sync input port structReturnSync: [3] StructReturn

    }


    interface TypedPortsGuardedInputs {
        # ----------------------------------------------------------------------
        # Typed input ports with no return type
        # ----------------------------------------------------------------------

        guarded input port noArgsGuarded: [3] NoArgs

        guarded input port primitiveArgsGuarded: [3] PrimitiveArgs

        guarded input port stringArgsGuarded: [3] StringArgs

        guarded input port enumArgsGuarded: [3] EnumArgs

        guarded input port arrayArgsGuarded: [3] ArrayArgs

        guarded input port structArgsGuarded: [3] StructArgs

        # ----------------------------------------------------------------------
        # Typed input ports with return type
        # ----------------------------------------------------------------------

        guarded input port noArgsReturnGuarded: [3] NoArgsReturn

        guarded input port primitiveReturnGuarded: [3] PrimitiveReturn

        guarded input port stringReturnGuarded: [3] StringReturn

        guarded input port stringAliasReturnGuarded: [3] StringAliasReturn

        guarded input port enumReturnGuarded: [3] EnumReturn

        guarded input port arrayReturnGuarded: [3] ArrayReturn

        guarded input port arrayStringAliasReturnGuarded: [3] ArrayStringAliasReturn

        guarded input port structReturnGuarded: [3] StructReturn

    }

    interface TypedPortsOutputs {
        # ----------------------------------------------------------------------
        # Typed output ports with no return type
        # ----------------------------------------------------------------------

        output port noArgsOut: [3] NoArgs

        output port primitiveArgsOut: [3] PrimitiveArgs

        output port stringArgsOut: [3] StringArgs

        output port enumArgsOut: [3] EnumArgs

        output port arrayArgsOut: [3] ArrayArgs

        output port structArgsOut: [3] StructArgs

        # ----------------------------------------------------------------------
        # Typed output ports with return type
        # ----------------------------------------------------------------------

        output port noArgsReturnOut: [3] NoArgsReturn

        output port primitiveReturnOut: [3] PrimitiveReturn

        output port stringReturnOut: [3] StringReturn

        output port stringAliasReturnOut: [3] StringAliasReturn

        output port enumReturnOut: [3] EnumReturn

        output port arrayReturnOut: [3] ArrayReturn

        output port arrayStringAliasReturnOut: [3] ArrayStringAliasReturn

        output port structReturnOut: [3] StructReturn

    }

    interface TypedPortsHelpers {
        # ----------------------------------------------------------------------
        # Ports for testing special ports
        # ----------------------------------------------------------------------

        output port prmGetIn: Fw.PrmGet

        output port prmSetIn: Fw.PrmSet

    }
}