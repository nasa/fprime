module FppTest {
    interface TypedPortsSyncInputs {
        # ----------------------------------------------------------------------
        # Typed input ports with no return type
        # ----------------------------------------------------------------------

        sync input port noArgsSync: [2] NoArgs

        sync input port primitiveArgsSync: [2] PrimitiveArgs

        sync input port stringArgsSync: [2] StringArgs

        sync input port enumArgsSync: [2] EnumArgs

        sync input port arrayArgsSync: [2] ArrayArgs

        sync input port structArgsSync: [2] StructArgs

        # ----------------------------------------------------------------------
        # Typed input ports with return type
        # ----------------------------------------------------------------------

        sync input port noArgsReturnSync: [2] NoArgsReturn

        sync input port primitiveReturnSync: [2] PrimitiveReturn

        sync input port stringReturnSync: [2] StringReturn

        sync input port stringAliasReturnSync: [2] StringAliasReturn

        sync input port enumReturnSync: [2] EnumReturn

        sync input port arrayReturnSync: [2] ArrayReturn

        sync input port arrayStringAliasReturnSync: [2] ArrayStringAliasReturn

        sync input port structReturnSync: [2] StructReturn

    }


    interface TypedPortsGuardedInputs {
        # ----------------------------------------------------------------------
        # Typed input ports with no return type
        # ----------------------------------------------------------------------

        guarded input port noArgsGuarded: [2] NoArgs

        guarded input port primitiveArgsGuarded: [2] PrimitiveArgs

        guarded input port stringArgsGuarded: [2] StringArgs

        guarded input port enumArgsGuarded: [2] EnumArgs

        guarded input port arrayArgsGuarded: [2] ArrayArgs

        guarded input port structArgsGuarded: [2] StructArgs

        # ----------------------------------------------------------------------
        # Typed input ports with return type
        # ----------------------------------------------------------------------

        guarded input port noArgsReturnGuarded: [2] NoArgsReturn

        guarded input port primitiveReturnGuarded: [2] PrimitiveReturn

        guarded input port stringReturnGuarded: [2] StringReturn

        guarded input port stringAliasReturnGuarded: [2] StringAliasReturn

        guarded input port enumReturnGuarded: [2] EnumReturn

        guarded input port arrayReturnGuarded: [2] ArrayReturn

        guarded input port arrayStringAliasReturnGuarded: [2] ArrayStringAliasReturn

        guarded input port structReturnGuarded: [2] StructReturn

    }

    interface TypedPortsOutputs {
        # ----------------------------------------------------------------------
        # Typed output ports with no return type
        # ----------------------------------------------------------------------

        output port noArgsOut: [2] NoArgs

        output port primitiveArgsOut: [2] PrimitiveArgs

        output port stringArgsOut: [2] StringArgs

        output port enumArgsOut: [2] EnumArgs

        output port arrayArgsOut: [2] ArrayArgs

        output port structArgsOut: [2] StructArgs

        # ----------------------------------------------------------------------
        # Typed output ports with return type
        # ----------------------------------------------------------------------

        output port noArgsReturnOut: [2] NoArgsReturn

        output port primitiveReturnOut: [2] PrimitiveReturn

        output port stringReturnOut: [2] StringReturn

        output port stringAliasReturnOut: [2] StringAliasReturn

        output port enumReturnOut: [2] EnumReturn

        output port arrayReturnOut: [2] ArrayReturn

        output port arrayStringAliasReturnOut: [2] ArrayStringAliasReturn

        output port structReturnOut: [2] StructReturn

    }

    interface TypedPortsHelpers {
        # ----------------------------------------------------------------------
        # Ports for testing special ports
        # ----------------------------------------------------------------------

        output port prmGetIn: Fw.PrmGet

        output port prmSetIn: Fw.PrmSet

    }
}