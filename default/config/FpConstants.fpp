# ======================================================================
# FPP file for configuration of various F Prime framework constants
# ======================================================================

@ For the simple object registry provided with the framework, this specifies how many objects the registry will store.
constant FW_OBJ_SIMPLE_REG_ENTRIES = 500

@ When dumping the contents of the registry, this specifies the size of the buffer used to store object names.
@ Should be >= FW_OBJ_NAME_BUFFER_SIZE.
constant FW_OBJ_SIMPLE_REG_BUFF_SIZE = 255

@ For the simple queue registry provided with the framework, this specifies how many queues the registry will store.
constant FW_QUEUE_SIMPLE_QUEUE_ENTRIES = 100

@ Specifies the size of the string holding the queue name for queues
constant FW_QUEUE_NAME_BUFFER_SIZE = 80

@ Specifies the size of the string holding the task name for active components and tasks
constant FW_TASK_NAME_BUFFER_SIZE = 80

@ Specifies the size of the buffer that contains a communications packet
constant FW_COM_BUFFER_MAX_SIZE = 512

@ Specifies the size of the buffer attached to state machine signals
constant FW_SM_SIGNAL_BUFFER_MAX_SIZE = 128

@ Specifies the size of the buffer that contains the serialized command arguments
constant FW_CMD_ARG_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)  # TODO: switch `- 4 - 4` with `- sizeof(FwOpcodeType) - sizeof(FwPacketDescriptorType))`

@ Specifies the maximum size of a string in a command argument
constant FW_CMD_STRING_MAX_SIZE = 40

@ Specifies the size of the buffer that contains the serialized log arguments
constant FW_LOG_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)  # TODO: 4 is for `- sizeof(FwEventIdType) - sizeof(FwPacketDescriptorType))`

@ Specifies the maximum size of a string in a log event
@ Note: This constant truncates file names in assertion failure event reports
constant FW_LOG_STRING_MAX_SIZE = 200

@ Specifies the size of the buffer that contains the serialized telemetry value
constant FW_TLM_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)  # TODO: 4 is for some sizeofs

@ Specifies the size of the buffer that contains the serialized telemetry value
constant FW_STATEMENT_ARG_BUFFER_MAX_SIZE = FW_CMD_ARG_BUFFER_MAX_SIZE

@ Specifies the maximum size of a string in a telemetry channel
constant FW_TLM_STRING_MAX_SIZE = 40

@ Specifies the size of the buffer that contains the serialized parameter value
constant FW_PARAM_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)  # TODO: 4 is for some sizeofs

@ Specifies the maximum size of a string in a parameter
constant FW_PARAM_STRING_MAX_SIZE = 40

@ Specifies the maximum size of a file downlink chunk
constant FW_FILE_BUFFER_MAX_SIZE = FW_COM_BUFFER_MAX_SIZE

@ Specifies the maximum size of a string in an interface call
constant FW_INTERNAL_INTERFACE_STRING_MAX_SIZE = 256

@ Defines the size of the text log string buffer. Should be large enough for format string and arguments
constant FW_LOG_TEXT_BUFFER_SIZE = 256

@ Configuration for Fw::String
@ Note: FPrimeBasicTypes.hpp needs to be updated to sync enum
constant FW_FIXED_LENGTH_STRING_SIZE = 256
