# ----------------------------------------------------------------------
# Parameters
# ----------------------------------------------------------------------

@ A parameter with U32 data
param ParamU32: U32

@ A parameter with F64 data
param ParamF64: F64

@ A parameter with string data and default value
param ParamString: string \
  default "default"

@ A parameter with enum data
param ParamEnum: FormalParamEnum \
  id 0x30

@ A parameter with array data, default value, and save opcode
param ParamArray: FormalParamArray \
  default [ 1.0, 2.0, 3.0 ] \
  save opcode 0x35

@ A parameter with struct data and set/save opcodes
param ParamStruct: FormalParamStruct \
  set opcode 0x40 \
  save opcode 0x45
