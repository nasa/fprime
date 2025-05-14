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
    # fpp requires we have something in a struct
    _empty: U8
}

struct GetTlmDirective {
    @ the local variable to store the tlm value in
    valueDestLvar: U8
    @ the local variable to store the tlm time in
    timeDestLvar: U8
    @ the tlm channel id to get the time of
    chanId: FwChanIdType
}

struct GetPrmDirective {
    @ the local variable to store the prm value in
    destLvarIndex: U8
    @ the param id to get the value of
    prmId: FwPrmIdType
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_setLocalVar(directive: SetLocalVarDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert

internal port directive_getTlm(directive: GetTlmDirective) priority 6 assert

internal port directive_getPrm(directive: GetPrmDirective) priority 6 assert