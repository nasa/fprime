# ----------------------------------------------------------------------
# General ports
# ----------------------------------------------------------------------

@ A typed sync input port
sync input port noArgsSync: [3] NoArgs

@ A typed guarded input
guarded input port noArgsGuarded: NoArgs

@ A typed sync input port
sync input port noArgsReturnSync: [3] NoArgsReturn

@ A typed guarded input
guarded input port noArgsReturnGuarded: NoArgsReturn

@ A typed sync input port
sync input port typedSync: [3] Typed

@ A typed guarded input
guarded input port typedGuarded: Typed

@ A typed sync input port with a return type
sync input port typedReturnSync: [3] TypedReturn

@ A typed guarded input with a return type
guarded input port typedReturnGuarded: TypedReturn

@ A typed output port
output port typedOut: Typed

@ A typed output port with a return type
output port typedReturnOut: TypedReturn
