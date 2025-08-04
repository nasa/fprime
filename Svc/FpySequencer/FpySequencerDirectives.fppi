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
    argBuf: [Fpy.MAX_DIRECTIVE_SIZE] U8

    @ the length of the arg buf byte array
    _argBufSize: FwSizeType
}

@ generic stack operation handler
struct StackOpDirective {
    @ which stack op to perform
    _op: Fpy.DirectiveId
}

struct ExitDirective {
    # pops U8 off the stack
    _empty: U8
}

@ pushes some empty bytes to the stack
struct AllocateDirective {
    $size: U16
}

@ pops some bytes off the stack and puts them in lvar array
struct StoreDirective {
    lvarOffset: U16
    $size: U16
}

@ gets bytes from lvar array and pushes them to stack
struct LoadDirective {
    lvarOffset: U16
    $size: U16
}

@ pushes a const byte array onto stack
struct PushValDirective {
    @ the byte array to push
    val: [Fpy.MAX_DIRECTIVE_SIZE] U8

    @ the length of the val buf
    _valSize: FwSizeType
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

internal port directive_allocate(directive: AllocateDirective) priority 6 assert

internal port directive_store(directive: StoreDirective) priority 6 assert

internal port directive_load(directive: LoadDirective) priority 6 assert

internal port directive_pushVal(directive: PushValDirective) priority 6 assert

enum DirectiveErrorCode {
    NO_ERROR
    STMT_OUT_OF_BOUNDS
    TLM_GET_NOT_CONNECTED
    TLM_CHAN_NOT_FOUND
    PRM_GET_NOT_CONNECTED
    PRM_NOT_FOUND
    CMD_SERIALIZE_FAILURE
    DELIBERATE_FAILURE
    STACK_ACCESS_OUT_OF_BOUNDS
    STACK_OVERFLOW
}