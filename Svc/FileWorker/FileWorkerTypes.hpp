// ======================================================================
// \title  FileWorkerTypes.hpp
// \author racheljt
// \brief  hpp file for FileWorker component types
// ======================================================================

#ifndef SVC_FILEWORKER_FILEWORKERTYPES_HPP_
#define SVC_FILEWORKER_FILEWORKERTYPES_HPP_

namespace Svc {

typedef enum { FW_STATE_IDLE = 0, FW_STATE_WRITING, FW_STATE_READING } FileWorkerState;

typedef enum {
    FW_STATUS_NOT_IDLE,
    FW_STATUS_FILE_NOT_OPEN,
    FW_STATUS_FAILED_TO_OPEN,
    FW_STATUS_FAILED_TO_WRITE,
    FW_STATUS_FAILED_TO_READ,
    FW_STATUS_FAILED_TO_START,
    FW_STATUS_CANCELLED_WRITE,
    FW_STATUS_FAILED_CRC,
    FW_STATUS_FAILED_FILE_SIZE,
    FW_STATUS_FAILED_FILE_SIZE_CONV,
    FW_STATUS_FAILED_GET_BUF,
    FW_STATUS_START_WRITE,
    FW_STATUS_DONE_WRITE,
    FW_STATUS_START_READ,
    FW_STATUS_DONE_READ,
    FW_STATUS_DONE
} FileWorkerStatus;

typedef enum { FW_READ_DONE = 0, FW_READ_ERROR, FW_READ_ABORT, FW_READ_TIMEOUT, FW_READ_UNKNOWN } FileWorkerReadStatus;

}  // namespace Svc

#endif /* SVC_FILEWORKER_FILEWORKERTYPES_HPP_ */
