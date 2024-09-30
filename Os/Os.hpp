// ======================================================================
// \title Os/Os.hpp
// \brief common definitions for the OSAL layer
// ======================================================================
#ifndef OS_OS_HPP_
#define OS_OS_HPP_
#include "FpConfig.h"

//! Storage type for OSAL handles
typedef U8 QueueHandleStorage[FW_QUEUE_HANDLE_MAX_SIZE];
typedef U8 HandleStorage[FW_HANDLE_MAX_SIZE];
typedef U8 DirectoryHandleStorage[FW_DIRECTORY_HANDLE_MAX_SIZE];
typedef U8 FileSystemHandleStorage[FW_FILESYSTEM_HANDLE_MAX_SIZE];
typedef U8 ConditionVariableHandleStorage[FW_CONDITION_VARIABLE_HANDLE_MAX_SIZE];

namespace Os {

//! \brief Initialization function for the OSAL layer
void init();

}  // namespace Os
#endif
