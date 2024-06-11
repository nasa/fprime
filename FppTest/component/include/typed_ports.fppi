# ----------------------------------------------------------------------
# Typed input ports with no return type
# ----------------------------------------------------------------------

sync input port noArgsSync: [2] NoArgs

guarded input port noArgsGuarded: [2] NoArgs

sync input port primitiveArgsSync: [2] PrimitiveArgs

guarded input port primitiveArgsGuarded: [2] PrimitiveArgs

sync input port stringArgsSync: [2] StringArgs

guarded input port stringArgsGuarded: [2] StringArgs

sync input port enumArgsSync: [2] EnumArgs

guarded input port enumArgsGuarded: [2] EnumArgs

sync input port arrayArgsSync: [2] ArrayArgs

guarded input port arrayArgsGuarded: [2] ArrayArgs

sync input port structArgsSync: [2] StructArgs

guarded input port structArgsGuarded: [2] StructArgs

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
# Typed input ports with return type
# ----------------------------------------------------------------------

sync input port noArgsReturnSync: NoArgsReturn

guarded input port noArgsReturnGuarded: NoArgsReturn

sync input port primitiveReturnSync: PrimitiveReturn

guarded input port primitiveReturnGuarded: PrimitiveReturn

# sync input port stringReturnSync: StringReturn
 
# guarded input port stringReturnGuarded: StringReturn

sync input port enumReturnSync: EnumReturn

guarded input port enumReturnGuarded: EnumReturn

sync input port arrayReturnSync: ArrayReturn

guarded input port arrayReturnGuarded: ArrayReturn

sync input port structReturnSync: StructReturn

guarded input port structReturnGuarded: StructReturn

# ----------------------------------------------------------------------
# Typed output ports with return type
# ----------------------------------------------------------------------

output port noArgsReturnOut: NoArgsReturn

output port primitiveReturnOut: PrimitiveReturn

# output port stringReturnOut: StringReturn

output port enumReturnOut: EnumReturn

output port arrayReturnOut: ArrayReturn

output port structReturnOut: StructReturn

# ----------------------------------------------------------------------
# Ports for testing special ports
# ----------------------------------------------------------------------

output port prmGetIn: Fw.PrmGet

output port prmSetIn: Fw.PrmSet
