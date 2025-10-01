external param ParamBoolExternal: bool

external param ParamI32External: I32

external param ParamStringExternal: string \
  default "external default"

external param ParamEnumExternal: FormalParamEnum \
  id 0x50

external param ParamArrayExternal: FormalParamArray \
  default [ 1.0, 2.0, 3.0 ] \
  save opcode 0x75

external param ParamStructExternal: FormalParamStruct \
  set opcode 0x80 \
  save opcode 0x85
