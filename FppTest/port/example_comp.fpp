@ An example component
passive component Example {

  # Typed ports with no return types

  sync input port noArgsIn: NoArgs
  
  output port noArgsOut: NoArgs
  
  sync input port primitiveArgsIn: PrimitiveArgs
  
  output port primitiveArgsOut: PrimitiveArgs

  sync input port stringArgsIn: StringArgs
  
  output port stringArgsOut: StringArgs

  sync input port enumArgsIn: EnumArgs

  output port enumArgsOut: EnumArgs

  sync input port arrayArgsIn: ArrayArgs

  output port arrayArgsOut: ArrayArgs

  sync input port structArgsIn: StructArgs

  output port structArgsOut: StructArgs

  # Typed ports with return types

  sync input port noArgsReturnIn: NoArgsReturn

  output port noArgsReturnOut: NoArgsReturn
  
  sync input port primitiveReturnIn: PrimitiveReturn
  
  output port primitiveReturnOut: PrimitiveReturn

  sync input port stringReturnIn: StringReturn
  
  output port stringReturnOut: StringReturn

  sync input port enumReturnIn: EnumReturn

  output port enumReturnOut: EnumReturn

  sync input port arrayReturnIn: ArrayReturn

  output port arrayReturnOut: ArrayReturn

  sync input port structReturnIn: StructReturn

  output port structReturnOut: StructReturn

  # Typed ports for testing with serial ports

  sync input port noArgsToSerial: NoArgs

  sync input port primitiveArgsToSerial: PrimitiveArgs

  sync input port stringArgsToSerial: StringArgs

  sync input port enumArgsToSerial: EnumArgs

  sync input port arrayArgsToSerial: ArrayArgs

  sync input port structArgsToSerial: StructArgs

  # Serial ports

  sync input port serialToSerial: serial

  sync input port serialToNoArgs: serial

  sync input port serialToStringArgs: serial

  sync input port serialToEnumArgs: serial

  sync input port serialToArrayArgs: serial

  sync input port serialToStructArgs: serial

  output port serialOut: serial

}
