# =======================================================================
# FPP file for configuration of various F Prime Platform and Os constants
# =======================================================================

@ Maximum size of a handle for OS queues
constant FW_CONSOLE_HANDLE_MAX_SIZE = 24

@ Maximum size of a handle for OS queues
constant FW_TASK_HANDLE_MAX_SIZE = 24

@ Maximum size of a handle for OS queues
constant FW_FILE_HANDLE_MAX_SIZE = 16

@ Maximum size of a handle for OS queues
constant FW_MUTEX_HANDLE_MAX_SIZE = 72

@ Maximum size of a handle for OS queues
constant FW_QUEUE_HANDLE_MAX_SIZE = 352

@ Maximum size of a handle for OS resources (files, queues, locks, etc.)
constant FW_DIRECTORY_HANDLE_MAX_SIZE = 16

@ Maximum size of a handle for OS resources (files, queues, locks, etc.)
constant FW_FILESYSTEM_HANDLE_MAX_SIZE = 16

@ Maximum size of a handle for OS::RawTime objects
constant FW_RAW_TIME_HANDLE_MAX_SIZE = 56

@ Maximum allowed serialization size for Os::RawTime objects
constant FW_RAW_TIME_SERIALIZATION_MAX_SIZE = 8

@ Maximum size of a handle for OS condition variables
constant FW_CONDITION_VARIABLE_HANDLE_MAX_SIZE = 56

@ Maximum size of a handle for OS cpu
constant FW_CPU_HANDLE_MAX_SIZE = 16

@ Maximum size of a handle for OS memory
constant FW_MEMORY_HANDLE_MAX_SIZE = 16

@ Alignment of handle storage
constant FW_HANDLE_ALIGNMENT = 8

@ Chunk size for working with files in the OSAL layer
constant FW_FILE_CHUNK_SIZE = 512
