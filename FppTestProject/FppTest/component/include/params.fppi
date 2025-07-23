param ParamBool: bool

param ParamU32: U32

param ParamString: string \
  default "default"

param ParamEnum: FormalParamEnum \
  id 0x30

param ParamArray: FormalParamArray \
  default [ 1.0, 2.0, 3.0 ] \
  save opcode 0x35

param ParamStruct: FormalParamStruct \
  set opcode 0x40 \
  save opcode 0x45
