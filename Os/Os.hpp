// ======================================================================
// \title Os/Os.hpp
// \brief common definitions for the OSAL layer
// ======================================================================
#ifndef OS_OS_HPP_
#define OS_OS_HPP_
#include "FpConfig.h"

//! Storage type for OSAL handles
typedef U8 QueueHandleStorage[FW_QUEUE_HANDLE_MAX_SIZE];
typedef U8 ConsoleHandleStorage[FW_CONSOLE_HANDLE_MAX_SIZE];
typedef U8 MutexHandleStorage[FW_MUTEX_HANDLE_MAX_SIZE];
typedef U8 FileHandleStorage[FW_FILE_HANDLE_MAX_SIZE];
typedef U8 TaskHandleStorage[FW_TASK_HANDLE_MAX_SIZE];
typedef U8 DirectoryHandleStorage[FW_DIRECTORY_HANDLE_MAX_SIZE];
typedef U8 FileSystemHandleStorage[FW_FILESYSTEM_HANDLE_MAX_SIZE];
typedef U8 ConditionVariableHandleStorage[FW_CONDITION_VARIABLE_HANDLE_MAX_SIZE];
typedef U8 CpuHandleStorage[FW_CPU_HANDLE_MAX_SIZE];
typedef U8 MemoryHandleStorage[FW_MEMORY_HANDLE_MAX_SIZE];

namespace Os {
namespace Generic {
//! Generic OK/ERROR status
enum Status {
    OP_OK, //!< Operation succeeded
    ERROR, //!< Operation failed
};

//! Generic used/total struct
struct UsedTotal {
    FwSizeType used; //!< Used amount
    FwSizeType total; //!< Total amount
};

}

//! \brief Initialization function for the OSAL layer
void init();

}  // namespace Os
#endif
