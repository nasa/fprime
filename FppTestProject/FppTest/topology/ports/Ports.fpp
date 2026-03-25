module FppTest {

  port Data(data: U32)

  enum SenderId {
    SYNC,
    GUARDED,
    ASYNC,
    N
  }

  port Reply(
    @ Port number called on the handler
    handlerPortNum: FwIndexType,

    @ The port handler that was invoked
    portId: TestDeploymentPort,

    @ Serialized arguments the handler was called with
    inputData: Fw.Buffer
  )

  @ Identifies the port handlers in the receiver to validate the proper
  @ port handler dispatch and connection in the topology code-generation
  enum TestDeploymentPort : U8 {
    INVALID,
    ARRAY_ARGS_ASYNC,
    ARRAY_ARGS_GUARDED,
    ARRAY_ARGS_SYNC,
    ARRAY_RETURN_GUARDED,
    ARRAY_RETURN_SYNC,
    ARRAY_STRING_ALIAS_RETURN_GUARDED,
    ARRAY_STRING_ALIAS_RETURN_SYNC,
    ENUM_ARGS_ASYNC,
    ENUM_ARGS_GUARDED,
    ENUM_ARGS_SYNC,
    ENUM_RETURN_GUARDED,
    ENUM_RETURN_SYNC,
    NO_ARGS_ASYNC,
    NO_ARGS_GUARDED,
    NO_ARGS_RETURN_GUARDED,
    NO_ARGS_RETURN_SYNC
    NO_ARGS_SYNC,
    PRIMITIVE_ARGS_ASYNC,
    PRIMITIVE_ARGS_GUARDED,
    PRIMITIVE_ARGS_SYNC,
    PRIMITIVE_RETURN_GUARDED,
    PRIMITIVE_RETURN_SYNC,
    STRING_ALIAS_RETURN_GUARDED,
    STRING_ALIAS_RETURN_SYNC,
    STRING_ARGS_ASYNC,
    STRING_ARGS_GUARDED,
    STRING_ARGS_SYNC,
    STRING_RETURN_GUARDED,
    STRING_RETURN_SYNC,
    STRUCT_ARGS_ASYNC,
    STRUCT_ARGS_GUARDED,
    STRUCT_ARGS_SYNC,
    STRUCT_RETURN_GUARDED,
    STRUCT_RETURN_SYNC,
  }

}
