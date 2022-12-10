@ An example component
passive component Example {

  # Typed input ports with no return types

  sync input port noArgsIn: [2] NoArgs
  
  sync input port primitiveArgsIn: [2] PrimitiveArgs
  
  sync input port stringArgsIn: [2] StringArgs
  
  sync input port enumArgsIn: [2] EnumArgs

  sync input port arrayArgsIn: [2] ArrayArgs

  sync input port structArgsIn: [2] StructArgs

  # Typed output ports with no return types

  output port noArgsOut: [2] NoArgs
  
  output port primitiveArgsOut: [2] PrimitiveArgs

  output port stringArgsOut: [2] StringArgs

  output port enumArgsOut: [2] EnumArgs

  output port arrayArgsOut: [2] ArrayArgs

  output port structArgsOut: [2] StructArgs

  # Typed input ports with return types

  sync input port noArgsReturnIn: NoArgsReturn

  sync input port primitiveReturnIn: PrimitiveReturn
  
  sync input port enumReturnIn: EnumReturn

  sync input port arrayReturnIn: ArrayReturn

  sync input port structReturnIn: StructReturn

  # Typed output ports with return types

  output port noArgsReturnOut: NoArgsReturn
  
  output port primitiveReturnOut: PrimitiveReturn

  output port enumReturnOut: EnumReturn

  output port arrayReturnOut: ArrayReturn

  output port structReturnOut: StructReturn

  # Serial ports

  sync input port serialIn: [7] serial

  output port serialOut: [7] serial

}
