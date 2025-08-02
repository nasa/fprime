struct WaitRelDirective {
    # pops U32, U32 off stack
    _empty: U8
}

struct WaitAbsDirective {
    # pops U32, U32, U8, U16 off stack
    _empty: U8
}

struct GotoDirective {
    @ the statement index to execute next
    statementIndex: U32
}

struct IfDirective {
    # pops U8 off stack
    @ the statement index to go to if the top of the stack is false
    falseGotoStmtIndex: U32
    # this directive will not goto anywhere if the variable is true
}

struct NoOpDirective {
    # fpp requires we have something in a struct
    _empty: U8
}

struct StoreTlmValDirective {
    @ the tlm channel id to get the time of
    chanId: FwChanIdType
    @ the offset in the lvar array to store the tlm in
    lvarOffset: U16
}

struct StorePrmDirective {
    @ the param id to get the value of
    prmId: FwPrmIdType
    @ the offset in the lvar array to store the prm in
    lvarOffset: U16
}

struct ConstCmdDirective {
    opCode: FwOpcodeType
    @ the arg buf of the cmd
    # TODO please don't let me merge this. need to find a better const here
    argBuf: [Fpy.MAX_CONST_CMD_SIZE] U8

    @ the length of the arg buf byte array
    _argBufSize: FwSizeType
}

@ generic stack operation handler
struct StackOpDirective {
    @ which stack op to perform
    _op: Fpy.DirectiveId
}

struct ExitDirective {
    @ whether or not to consider the sequence successful
    success: bool
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert

internal port directive_storeTlmVal(directive: StoreTlmValDirective) priority 6 assert

internal port directive_storePrm(directive: StorePrmDirective) priority 6 assert

internal port directive_constCmd(directive: ConstCmdDirective) priority 6 assert

internal port directive_stackOp(directive: StackOpDirective) priority 6 assert

internal port directive_exit(directive: ExitDirective) priority 6 assert

enum DirectiveErrorCode {
    NO_ERROR
    STMT_OUT_OF_BOUNDS
    TLM_GET_NOT_CONNECTED
    TLM_CHAN_NOT_FOUND
    PRM_GET_NOT_CONNECTED
    PRM_NOT_FOUND
    CMD_SERIALIZE_FAILURE
    DELIBERATE_FAILURE
    STACK_UNDERFLOW
    STACK_OVERFLOW
}