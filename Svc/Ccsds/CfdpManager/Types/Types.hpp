// ======================================================================
// \title  Types.hpp
// \author campuzan
// \brief  hpp file for shared CFDP protocol type definitions
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_Types_HPP
#define Svc_Ccsds_Cfdp_Types_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// CFDP File Directive Codes
// Blue Book section 5.2, table 5-4
enum FileDirective : U8 {
    FILE_DIRECTIVE_INVALID_MIN = 0,  // Minimum used to limit range
    FILE_DIRECTIVE_END_OF_FILE = 4,  // End of File
    FILE_DIRECTIVE_FIN = 5,          // Finished
    FILE_DIRECTIVE_ACK = 6,          // Acknowledge
    FILE_DIRECTIVE_METADATA = 7,     // Metadata
    FILE_DIRECTIVE_NAK = 8,          // Negative Acknowledge
    FILE_DIRECTIVE_PROMPT = 9,       // Prompt
    FILE_DIRECTIVE_KEEP_ALIVE = 12,  // Keep Alive
    FILE_DIRECTIVE_INVALID_MAX = 13  // Maximum used to limit range
};

// CFDP Condition Codes
// Blue Book section 5.2.2, table 5-5
enum ConditionCode : U8 {
    CONDITION_CODE_NO_ERROR = 0,
    CONDITION_CODE_POS_ACK_LIMIT_REACHED = 1,
    CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED = 2,
    CONDITION_CODE_INVALID_TRANSMISSION_MODE = 3,
    CONDITION_CODE_FILESTORE_REJECTION = 4,
    CONDITION_CODE_FILE_CHECKSUM_FAILURE = 5,
    CONDITION_CODE_FILE_SIZE_ERROR = 6,
    CONDITION_CODE_NAK_LIMIT_REACHED = 7,
    CONDITION_CODE_INACTIVITY_DETECTED = 8,
    CONDITION_CODE_INVALID_FILE_STRUCTURE = 9,
    CONDITION_CODE_CHECK_LIMIT_REACHED = 10,
    CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CONDITION_CODE_SUSPEND_REQUEST_RECEIVED = 14,
    CONDITION_CODE_CANCEL_REQUEST_RECEIVED = 15
};

// CFDP ACK Transaction Status
// Blue Book section 5.2.4, table 5-8
enum AckTxnStatus : U8 {
    ACK_TXN_STATUS_UNDEFINED = 0,
    ACK_TXN_STATUS_ACTIVE = 1,
    ACK_TXN_STATUS_TERMINATED = 2,
    ACK_TXN_STATUS_UNRECOGNIZED = 3
};

// CFDP FIN Delivery Code
// Blue Book section 5.2.3, table 5-7
enum FinDeliveryCode : U8 {
    FIN_DELIVERY_CODE_COMPLETE = 0,    // Data complete
    FIN_DELIVERY_CODE_INCOMPLETE = 1   // Data incomplete
};

// CFDP FIN File Status
// Blue Book section 5.2.3, table 5-7
enum FinFileStatus : U8 {
    FIN_FILE_STATUS_DISCARDED = 0,            // File discarded deliberately
    FIN_FILE_STATUS_DISCARDED_FILESTORE = 1,  // File discarded due to filestore rejection
    FIN_FILE_STATUS_RETAINED = 2,             // File retained successfully
    FIN_FILE_STATUS_UNREPORTED = 3            // File status unreported
};

// CFDP Checksum Type
// Blue Book section 5.2.5, table 5-9
enum ChecksumType : U8 {
    CHECKSUM_TYPE_MODULAR = 0,        // Modular checksum
    CHECKSUM_TYPE_CRC_32 = 1,         // CRC-32 (not currently supported)
    CHECKSUM_TYPE_NULL_CHECKSUM = 15  // Null checksum
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_Types_HPP
