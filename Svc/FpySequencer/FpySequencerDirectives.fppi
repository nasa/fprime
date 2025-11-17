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

@ pushes a tlm buf to the stack
struct PushTlmValDirective {
    @ the tlm channel id to get the time of
    chanId: FwChanIdType
}

@ pushes a tlm buffer to the stack, and then pushes the time 
@ of the tlm meas to the stack
struct PushTlmValAndTimeDirective {
    @ the tlm channel id to get the time and value of
    chanId: FwChanIdType
}

@ pushes a prm buf to the stack
struct PushPrmDirective {
    @ the param id to get the value of
    prmId: FwPrmIdType
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
struct StoreConstOffsetDirective {
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

@ pops bytes off the top of the stack and does nothing with them
struct DiscardDirective {
    @ how many bytes to pop off the stack
    $size: Fpy.StackSizeType
}

@ pop two byte arrays off the top of the stack, call memcmp, push 1 if they were equal, 0 otherwise
struct MemCmpDirective {
    @ how big a single byte array is. note, we pop 2x this amount off the stack
    $size: Fpy.StackSizeType
}

@ pop an opcode and arg buf off the stack, send to cmd dispatcher and await response
struct StackCmdDirective {
    @ how big the argument buffer is
    argsSize: Fpy.StackSizeType
}

@ pushes the current Fw.Time struct to the stack
struct PushTimeDirective {
    _empty: U8
}

@ pops a bool off the stack, sets a flag with a specific index to that bool
struct SetFlagDirective {
    flagIdx: U8
}

@ gets a flag and pushes its value as a U8 to the stack
struct GetFlagDirective {
    flagIdx: U8
}

@ grabs a specified region from a struct, pushes it to the stack, removing
@ the rest of the struct
struct GetFieldDirective {
    # pops an Fpy.StackSizeType offset in the parent off the stack
    @ total size of the struct
    parentSize: Fpy.StackSizeType
    @ size of the member to extract
    memberSize: Fpy.StackSizeType
}

@ peeks at N bytes from the stack, starting from an offset relative to the top of the stack
struct PeekDirective {
    # pops two StackSizeType off the stack
    _empty: U8
}

@ stores bytes from the top of the stack into a memory location, determined by the stack
struct StoreDirective {
    # pops an Fpy.StackSizeType offset off of stack
    @ size of data to pop off stack
    $size: Fpy.StackSizeType
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert

internal port directive_pushTlmVal(directive: PushTlmValDirective) priority 6 assert

internal port directive_pushTlmValAndTime(directive: PushTlmValAndTimeDirective) priority 6 assert

internal port directive_pushPrm(directive: PushPrmDirective) priority 6 assert

internal port directive_constCmd(directive: ConstCmdDirective) priority 6 assert

internal port directive_stackOp(directive: StackOpDirective) priority 6 assert

internal port directive_exit(directive: ExitDirective) priority 6 assert

internal port directive_allocate(directive: AllocateDirective) priority 6 assert

internal port directive_storeConstOffset(directive: StoreConstOffsetDirective) priority 6 assert

internal port directive_load(directive: LoadDirective) priority 6 assert

internal port directive_pushVal(directive: PushValDirective) priority 6 assert

internal port directive_discard(directive: DiscardDirective) priority 6 assert

internal port directive_memCmp(directive: MemCmpDirective) priority 6 assert

internal port directive_stackCmd(directive: StackCmdDirective) priority 6 assert

internal port directive_pushTime(directive: PushTimeDirective) priority 6 assert

internal port directive_setFlag(directive: SetFlagDirective) priority 6 assert

internal port directive_getFlag(directive: GetFlagDirective) priority 6 assert

internal port directive_getField(directive: GetFieldDirective) priority 6 assert

internal port directive_peek(directive: PeekDirective) priority 6 assert

internal port directive_store(directive: StoreDirective) priority 6 assert
