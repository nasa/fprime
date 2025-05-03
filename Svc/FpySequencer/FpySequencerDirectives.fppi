struct WaitRelDirective {
    duration: Fw.TimeInterval
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

# this directive should not be directly serialize/deserialized
struct SetLocalVarDirective {
    @ the index of the local variable to set
    index: U8

    @ the value of the local variable
    value: [Fpy.MAX_LOCAL_VARIABLE_BUFFER_SIZE] U8

    @ the length of the value byte array. 
    # note, the underscore signifies this field is implied and 
    # is not actually present in the binary form of the directive
    _valueSize: FwSizeType
}

struct GotoDirective {
    @ the statement index to execute next
    statementIndex: U32
}

struct IfDirective {
    @ the local variable to interpret as a boolean
    conditionalLocalVarIndex: U8
    @ the statement index to go to if the local variable is false
    falseGotoStmtIndex: U32
    # this directive will not goto anywhere if the variable is true
}

struct NoOpDirective {
    _empty: U8
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_setLocalVar(directive: SetLocalVarDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert