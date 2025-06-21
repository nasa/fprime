struct WaitRelDirective {
    seconds: U32
    uSeconds: U32
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

# this directive should not be directly serialize/deserialized
struct SetSerRegDirective {
    @ the index of the serializable register to set
    index: U8

    @ the value of the serializable register
    value: [Fpy.MAX_SERIALIZABLE_REGISTER_SIZE] U8

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
    @ the register to branch based off of. register will be interpreted as a C++ boolean
    conditionalReg: U8
    @ the statement index to go to if the serializable register is false
    falseGotoStmtIndex: U32
    # this directive will not goto anywhere if the variable is true
}

struct NoOpDirective {
    # fpp requires we have something in a struct
    _empty: U8
}

struct GetTlmDirective {
    @ the serializable register to store the tlm value in
    valueDestSerReg: U8
    @ the serializable register to store the tlm time in
    timeDestSerReg: U8
    @ the tlm channel id to get the time of
    chanId: FwChanIdType
}

struct GetPrmDirective {
    @ the serializable register to store the prm value in
    destSerRegIndex: U8
    @ the param id to get the value of
    prmId: FwPrmIdType
}

struct CmdDirective {
    opCode: FwOpcodeType
    @ the arg buf of the cmd
    # TODO please don't let me merge this. need to find a better const here
    argBuf: [Fpy.MAX_SERIALIZABLE_REGISTER_SIZE] U8

    @ the length of the arg buf byte array
    _argBufSize: FwSizeType
}

@ deser up to 8 bytes out of an serReg
struct DeserSerRegDirective {
    @ the serReg to deser from
    srcSerRegIdx: U8
    @ the start offset to deser from
    srcOffset: FwSizeType
    @ the destination register to deser into
    destReg: U8
    @ the number of bytes to deserialize from the serReg, max of 8
    _deserSize: U8
}

@ sets a register to a constant value
struct SetRegDirective {
    @ the register to store the value in
    dest: U8
    @ the value to store in the register
    value: I64
}

@ performs a binary comparison operator on the lhs and rhs regs, and stores the result in the third register
struct BinaryCmpDirective {
    lhs: U8
    rhs: U8
    res: U8
    
    @ which binary comparison op to perform
    _op: Fpy.DirectiveId
}

@ performs a binary complement on the src register, and stores the result in res
struct NotDirective {
    src: U8
    res: U8
}

struct ExitDirective {
    @ whether or not to consider the sequence successful
    success: bool
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_setSerReg(directive: SetSerRegDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_noOp(directive: NoOpDirective) priority 6 assert

internal port directive_getTlm(directive: GetTlmDirective) priority 6 assert

internal port directive_getPrm(directive: GetPrmDirective) priority 6 assert

internal port directive_cmd(directive: CmdDirective) priority 6 assert

internal port directive_deserSerReg(directive: DeserSerRegDirective) priority 6 assert

internal port directive_setReg(directive: SetRegDirective) priority 6 assert

internal port directive_binaryCmp(directive: BinaryCmpDirective) priority 6 assert

internal port directive_not(directive: NotDirective) priority 6 assert

internal port directive_exit(directive: ExitDirective) priority 6 assert

enum DirectiveErrorCode {
    NO_ERROR
    SER_REG_OUT_OF_BOUNDS
    STMT_OUT_OF_BOUNDS
    SER_REG_DESERIALIZE_FAILURE
    SER_REG_SERIALIZE_FAILURE
    TLM_GET_NOT_CONNECTED
    TLM_CHAN_NOT_FOUND
    PRM_GET_NOT_CONNECTED
    PRM_NOT_FOUND
    CMD_SERIALIZE_FAILURE
    REGISTER_OUT_OF_BOUNDS
    SER_REG_ACCESS_OUT_OF_BOUNDS
    DELIBERATE_FAILURE
}