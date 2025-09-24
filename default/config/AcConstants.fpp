# ======================================================================
# AcConstants.fpp
# F Prime configuration constants
# ======================================================================

@ Number of rate group member output ports for ActiveRateGroup
constant ActiveRateGroupOutputPorts = 10

@ Number of rate group member output ports for PassiveRateGroup
constant PassiveRateGroupOutputPorts = 10

@ Used to drive rate groups
constant RateGroupDriverRateGroupPorts = 3

@ Used for command and registration ports
constant CmdDispatcherComponentCommandPorts = 30

@ Used for uplink/sequencer buffer/response ports
constant CmdDispatcherSequencePorts = 5

@ Used for dispatching sequences to command sequencers
constant SeqDispatcherSequencerPorts = 2

@ Used for sizing the command splitter input arrays
constant CmdSplitterPorts = CmdDispatcherSequencePorts

@ Number of static memory allocations
constant StaticMemoryAllocations = 4

@ Used to ping active components
constant HealthPingPorts = 25

@ Used for broadcasting completed file downlinks
constant FileDownCompletePorts = 1

@ Used for number of Fw::Com type ports supported by Svc::ComQueue
constant ComQueueComPorts = 2

@ Used for number of Fw::Buffer type ports supported by Svc::ComQueue
constant ComQueueBufferPorts = 1

@ Used for maximum number of connected buffer repeater consumers
constant BufferRepeaterOutputPorts = 10

@ Size of port array for DpManager
constant DpManagerNumPorts = 5

@ Size of processing port array for DpWriter
constant DpWriterNumProcPorts = 5

@ The size of a file name string
constant FileNameStringSize = 200

@ The size of an assert text string
constant FwAssertTextSize = 256

@ The size of a file name in an AssertFatalAdapter event
@ Note: File names in assertion failures are also truncated by
@ the constants FW_ASSERT_TEXT_SIZE and FW_LOG_STRING_MAX_SIZE, set
@ in FpConfig.h.
constant AssertFatalAdapterEventFileSize = FileNameStringSize

# ----------------------------------------------------------------------
# Hub connections. Connections on all deployments should mirror these settings.
# ----------------------------------------------------------------------

constant GenericHubInputPorts = 10
constant GenericHubOutputPorts = 10
constant GenericHubInputBuffers = 10
constant GenericHubOutputBuffers = 10

# ----------------------------------------------------------------------
# Constants ported from FpConfig.h
# ----------------------------------------------------------------------

@ Dont care value for time contexts in sequences
constant FW_CONTEXT_DONT_CARE = 0xFF

@ Boolean values for serialization
constant FW_SERIALIZE_TRUE_VALUE = 0xFF                         # Value encoded during serialization for boolean true
constant FW_SERIALIZE_FALSE_VALUE = 0x00                        # Value encoded during serialization for boolean false

@ On some systems, use of *printf family functions (snprintf, printf, etc) require a prohibitive amount of program
@ space. Setting this to `0` indicates that the Fw/String methods should stop using these functions to conserve
@ program size. However, this comes at the expense of discarding format parameters. i.e. the format string is returned
@ unchanged.
constant FW_USE_PRINTF_FAMILY_FUNCTIONS_IN_STRING_FORMATTING = 1

@ For the simple object registry provided with the framework, this specifies how many objects the registry will store.
constant FW_OBJ_SIMPLE_REG_ENTRIES = 500                        # Number of objects stored in simple object registry

@ When dumping the contents of the registry, this specifies the size of the buffer used to store object names. Should
@ be >= FW_OBJ_NAME_BUFFER_SIZE.
constant FW_OBJ_SIMPLE_REG_BUFF_SIZE = 255                      # Size of object registry dump string

@ For the simple queue registry provided with the framework, this specifies how many queues the registry will store.
constant FW_QUEUE_SIMPLE_QUEUE_ENTRIES = 100                    # Number of queues stored in simple queue registry

@ Specifies the size of the string holding the queue name for queues
constant FW_QUEUE_NAME_BUFFER_SIZE = 80                         # Max size of message queue name

@ Specifies the size of the string holding the task name for active components and tasks
constant FW_TASK_NAME_BUFFER_SIZE = 80                          # Max size of task name

@ Specifies the size of the buffer that contains a communications packet
constant FW_COM_BUFFER_MAX_SIZE = 512

@ Specifies the size of the buffer attached to state machine signals
constant FW_SM_SIGNAL_BUFFER_MAX_SIZE = 128

@ Specifies the size of the buffer that contains the serialized command arguments
# FW_CMD_ARG_BUFFER_MAX_SIZE (FW_COM_BUFFER_MAX_SIZE - sizeof(FwOpcodeType) - sizeof(FwPacketDescriptorType))
constant FW_CMD_ARG_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)

@ Specifies the maximum size of a string in a command argument
constant FW_CMD_STRING_MAX_SIZE = 40                            # Max character size of command string arguments

@ Normally when a command is deserialized, the handler checks to see if there are any leftover
@ bytes in the buffer. If there are, it assumes that the command was corrupted somehow since
@ the serialized size should match the serialized size of the argument list. In some cases,
@ command buffers are padded so the data can be larger than the serialized size of the command.
@ Setting the below to zero will disable the check at the cost of not detecting commands that
@ are too large.
constant FW_CMD_CHECK_RESIDUAL = 1                              # Check for leftover command bytes

@ Specifies the size of the buffer that contains the serialized log arguments
# FW_LOG_BUFFER_MAX_SIZE (FW_COM_BUFFER_MAX_SIZE - sizeof(FwEventIdType) - sizeof(FwPacketDescriptorType))
constant FW_LOG_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)

@ Specifies the maximum size of a string in a log event
@ Note: This constant truncates file names in assertion failure event reports
constant FW_LOG_STRING_MAX_SIZE = 200                           # Max size of log string parameter type

@ Specifies the size of the buffer that contains the serialized telemetry value
constant FW_TLM_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)

@ Specifies the size of the buffer that contains the serialized telemetry value
constant FW_STATEMENT_ARG_BUFFER_MAX_SIZE = FW_CMD_ARG_BUFFER_MAX_SIZE

@ Specifies the maximum size of a string in a telemetry channel
constant FW_TLM_STRING_MAX_SIZE = 40                            # Max size of channelized telemetry string type

@ Specifies the size of the buffer that contains the serialized parameter value
constant FW_PARAM_BUFFER_MAX_SIZE = (FW_COM_BUFFER_MAX_SIZE - 4 - 4)

@ Specifies the maximum size of a string in a parameter
constant FW_PARAM_STRING_MAX_SIZE = 40                          # Max size of parameter string type

@ Specifies the maximum size of a file downlink chunk
constant FW_FILE_BUFFER_MAX_SIZE = FW_COM_BUFFER_MAX_SIZE       # Max size of file buffer (i.e. chunk of file)

@ Specifies the maximum size of a string in an interface call
constant FW_INTERNAL_INTERFACE_STRING_MAX_SIZE = 256            # Max size of interface string parameter type

@ Some settings to enable AMPCS compatibility. This breaks regular ISF GUI compatibility
constant FW_AMPCS_COMPATIBLE = 0                                # Whether or not JPL AMPCS ground system support is enabled

@ Configuration for Fw::String
@ Note: FPrimeBasicTypes.hpp needs to be updated to sync enum
constant FW_FIXED_LENGTH_STRING_SIZE = 256                      # Character array size for Fw::String

@ OS configuration
constant FW_CONSOLE_HANDLE_MAX_SIZE = 24                        # Maximum size of a handle for OS queues

constant FW_TASK_HANDLE_MAX_SIZE = 24                           # Maximum size of a handle for OS queues

constant FW_FILE_HANDLE_MAX_SIZE = 16                           # Maximum size of a handle for OS queues

constant FW_MUTEX_HANDLE_MAX_SIZE = 72                          # Maximum size of a handle for OS queues

constant FW_QUEUE_HANDLE_MAX_SIZE = 352                         # Maximum size of a handle for OS queues

constant FW_DIRECTORY_HANDLE_MAX_SIZE = 16                      # Maximum size of a handle for OS resources (files, queues, locks, etc.)

constant FW_FILESYSTEM_HANDLE_MAX_SIZE = 16                     # Maximum size of a handle for OS resources (files, queues, locks, etc.)

constant FW_RAW_TIME_HANDLE_MAX_SIZE = 56                       # Maximum size of a handle for OS::RawTime objects

constant FW_RAW_TIME_SERIALIZATION_MAX_SIZE = 8                 # Maximum allowed serialization size for Os::RawTime objects

constant FW_CONDITION_VARIABLE_HANDLE_MAX_SIZE = 56             # Maximum size of a handle for OS condition variables

constant FW_CPU_HANDLE_MAX_SIZE = 16                            # Maximum size of a handle for OS cpu

constant FW_MEMORY_HANDLE_MAX_SIZE = 16                         # Maximum size of a handle for OS memory

constant FW_HANDLE_ALIGNMENT = 8                                # Alignment of handle storage

constant FW_FILE_CHUNK_SIZE = 512                               # Chunk size for working with files in the OSAL layer

constant FW_ASSERT_COUNT_MAX = 10                               # Maximum number of cascading FW_ASSERT check failures before forcing a system assert
