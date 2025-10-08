@ sleeps for a relative duration from the current time
struct WaitRelDirective {
    # pops U32, U32 off stack
    _empty: U8
}

@ sleeps until an absolute time
struct WaitAbsDirective {
    # pops U32, U32, U8, U16 off stack
    _empty: U8
}

@ sets the index of the next directive to execute
struct GotoDirective {
    @ the statement index to execute next
    statementIndex: U32
}

@ branches based off of the top byte of the stack
struct IfDirective {
    # pops U8 off stack
    @ the statement index to go to if the top of the stack is false
    falseGotoStmtIndex: U32
    # this directive will not goto anywhere if the variable is true
}

@ does nothing
struct NoOpDirective {
    # fpp requires we have something in a struct
    _empty: U8
}

@ stores a tlm buffer in the lvar array
struct StoreTlmValDirective {
    @ the tlm channel id to get the time of
    chanId: FwChanIdType
    @ the offset in the lvar array to store the tlm in
    lvarOffset: Fpy.StackSizeType
}

@ pushes a tlm buffer to the stack, and then pushes the time 
@ of the tlm meas to the stack
struct PushTlmValAndTimeDirective {
    @ the tlm channel id to get the time and value of
    chanId: FwChanIdType
}

@ stores a prm buffer in the lvar array
struct StorePrmDirective {
    @ the param id to get the value of
    prmId: FwPrmIdType
    @ the offset in the lvar array to store the prm in
    lvarOffset: Fpy.StackSizeType
}

@ executes a cmd with const args
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

@ ends the sequence
struct ExitDirective {
    # pops U8 off the stack
    _empty: U8
}

@ pushes some empty bytes to the stack
struct AllocateDirective {
    $size: Fpy.StackSizeType
}

@ pops some bytes off the stack and puts them in lvar array
struct StoreDirective {
    lvarOffset: Fpy.StackSizeType
    $size: Fpy.StackSizeType
}

@ gets bytes from lvar array and pushes them to stack
struct LoadDirective {
    lvarOffset: Fpy.StackSizeType
    $size: Fpy.StackSizeType
}

@ pushes a const byte array onto stack
struct PushValDirective {
    @ the byte array to push
    val: [Fpy.MAX_DIRECTIVE_SIZE] U8

    @ the length of the val buf
    _valSize: FwSizeType
}

struct DiscardDirective {
    $size: Fpy.StackSizeType
}

struct MemCmpDirective {
    $size: Fpy.StackSizeType
}

struct StackCmdDirective {
    argsSize: Fpy.StackSizeType
}

@ pushes the current Fw.Time struct to the stack
struct PushTimeDirective {
    _empty: U8
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert

internal port directive_storeTlmVal(directive: StoreTlmValDirective) priority 6 assert

internal port directive_pushTlmValAndTime(directive: PushTlmValAndTimeDirective) priority 6 assert

internal port directive_storePrm(directive: StorePrmDirective) priority 6 assert

internal port directive_constCmd(directive: ConstCmdDirective) priority 6 assert

internal port directive_stackOp(directive: StackOpDirective) priority 6 assert

internal port directive_exit(directive: ExitDirective) priority 6 assert

internal port directive_allocate(directive: AllocateDirective) priority 6 assert

internal port directive_store(directive: StoreDirective) priority 6 assert

internal port directive_load(directive: LoadDirective) priority 6 assert

internal port directive_pushVal(directive: PushValDirective) priority 6 assert

internal port directive_discard(directive: DiscardDirective) priority 6 assert

internal port directive_memCmp(directive: MemCmpDirective) priority 6 assert

internal port directive_stackCmd(directive: StackCmdDirective) priority 6 assert

internal port directive_pushTime(directive: PushTimeDirective) priority 6 assert

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
    DOMAIN_ERROR
}