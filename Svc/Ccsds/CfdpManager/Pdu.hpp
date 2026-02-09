// ======================================================================
// \title  CfdpPdu.hpp
// \brief  Structures defining CFDP PDUs
//
// This file is a port of CFDP PDU structures from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp_pdu.h (CFDP PDU structure definitions per CCSDS 727.0-B-5)
//
// The structures and enumerations defined in this file with a Cfdp
// prefix are defined according to the CCSDS CFDP specification (727.0-B-5).
// These values must match the specification for that structure/field, they are
// not locally changeable.
//
// Many of the structures defined in this file are variably-sized when
// encoded for network transmission.  As a result, C structures used to map
// to these structures are of limited usefulness, generally only capable
// of describing the first element(s) where offsets are fixed.  A marker member
// is utilized to indicate where the fixed data ends and variable
// length data begins.  At some point, the structures in this file
// should change to encode/decode functions.
//
// ======================================================================
//
// NASA Docket No. GSC-18,447-1
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ======================================================================

#ifndef CFDP_PDU_HPP
#define CFDP_PDU_HPP

#include <stddef.h>

#include <config/CfdpCfg.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

/**
 * @brief Minimum encoded size of a CFDP PDU header
 *
 * Per the blue book, the size of the Entity ID and Sequence Number must be at least 1 byte.
 */
#define CFDP_MIN_HEADER_SIZE \
    (sizeof(PduHeader) + (3 * sizeof(U8Type))) /* 1 byte for each variable item */

/**
 * @brief Maximum encoded size of a CFDP PDU that this implementation can accept
 *
 * This definition reflects the current configuration of the CFDP implementation.
 * Note that this is based on the size of the native representation of Entity ID and
 * sequence number.  Although the bitwise representations of these items are
 * different in the encoded packets vs. the native representation, the basic size
 * still correlates (e.g. if it takes 4 bytes natively, it will be encoded into
 * 4 bytes).
 */
#define CFDP_APP_MAX_HEADER_SIZE (sizeof(PduHeader) + sizeof(TransactionSeq) + (3 * sizeof(EntityId)))

/*
 * CFDP PDU data types are based on wrapper structs which
 * accomplish two things:
 *  1. Attempts to read/write directly as numbers will trigger
 *     a compiler error - one must use the access macros.
 *  2. Values are unaligned, and will not induce any alignment
 *     padding - basically making the structs "packed".
 *
 * Many of the values within CFDP PDUs have some sort of bitfield
 * or special encoding.  It is the responsibility of the codec
 * routines to translate these bits into logical values.  This
 * is why direct access to these bits is discouraged - there is
 * always some translation required in order to use them.
 */

/**
 * @brief Encoded 8-bit value in the CFDP PDU
 */
struct U8Type
{
    U8 octets[1];
};

/**
 * @brief Encoded 16-bit value in the CFDP PDU
 */
struct U16Type
{
    U8 octets[2];
};

/**
 * @brief Encoded 32-bit value in the CFDP PDU
 */
struct U32Type
{
    U8 octets[4];
};

/**
 * @brief Encoded 64-bit value in the CFDP PDU
 */
struct U64Type
{
    U8 octets[8];
};

/**
 * @brief Structure representing base CFDP PDU header
 *
 * This header appears at the beginning of all CFDP PDUs, of all types.
 * Note that the header is variable length, it also contains source
 * and destination entity IDs, and the transaction sequence number.
 *
 * Defined per section 5.1 of CCSDS 727.0-B-5
 *
 * @note this contains variable length data for the EID+TSN, which is _not_ included
 * in this definition.  As a result, the sizeof(PduHeader) reflects only the
 * size of the fixed fields.  The actual size includes the variable length fields.
 */
#if 0  // Replaced by Types/PduHeader.hpp class implementation
struct PduHeader
{
    U8Type  flags;           /**< \brief Flags indicating the PDU type, direction, mode, etc */
    U16Type length;          /**< \brief Length of the entire PDU, in octets */
    U8Type  eid_tsn_lengths; /**< \brief Lengths of the EID+TSN data (bitfields) */

    /* variable-length data goes here - it is at least 3 additional bytes */
};
#endif

/**
 * @brief Structure representing CFDP File Directive Header
 *
 * Defined per section 5.2 of CCSDS 727.0-B-5
 */
struct PduFileDirectiveHeader
{
    U8Type directive_code;
};

/**
 * @brief Structure representing CFDP LV Object format
 *
 * These Length + Value pairs used in several CFDP PDU types,
 * typically for storage of strings such as file names.
 *
 * Defined per table 5-2 of CCSDS 727.0-B-5
 */
#if 0  // Old struct definition
struct Lv
{
    U8Type length; /**< \brief Length of data field */
};
#endif

/**
 * @brief Structure representing CFDP TLV Object format
 *
 * These Type + Length + Value pairs used in several CFDP PDU types,
 * typically for file storage requests (section 5.4).
 *
 * Defined per table 5-3 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/Tlv.hpp class implementation
struct Tlv
{
    U8Type type;   /**< \brief Nature of data field */
    U8Type length; /**< \brief Length of data field */
};
#endif

/**
 * @brief Values for "acknowledgment transfer status"
 *
 * This enum is pertinent to the ACK PDU type, defines the
 * values for the directive field.
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/Types.hpp
enum AckTxnStatus : U8
{
    ACK_TXN_STATUS_UNDEFINED    = 0,
    ACK_TXN_STATUS_ACTIVE       = 1,
    ACK_TXN_STATUS_TERMINATED   = 2,
    ACK_TXN_STATUS_UNRECOGNIZED = 3,
    ACK_TXN_STATUS_INVALID      = 4,
};
#endif

/**
 * @brief Values for "finished delivery code"
 *
 * This enum is pertinent to the FIN PDU type, defines the
 * values for the delivery code field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/Types.hpp
enum FinDeliveryCode : U8
{
    FIN_DELIVERY_CODE_COMPLETE   = 0,
    FIN_DELIVERY_CODE_INCOMPLETE = 1,
    FIN_DELIVERY_CODE_INVALID    = 2,
};
#endif

/**
 * @brief Values for "finished file status"
 *
 * This enum is pertinent to the FIN PDU type, defines the
 * values for the file status field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/Types.hpp
enum FinFileStatus : U8
{
    FIN_FILE_STATUS_DISCARDED           = 0,
    FIN_FILE_STATUS_DISCARDED_FILESTORE = 1,
    FIN_FILE_STATUS_RETAINED            = 2,
    FIN_FILE_STATUS_UNREPORTED          = 3,
    FIN_FILE_STATUS_INVALID             = 4,
};
#endif

/**
 * @brief Values for "condition code"
 *
 * This enum defines the values for the condition code field
 * for the PDU types which have this field (EOF, FIN, ACK)
 *
 * Defined per table 5-5 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/Types.hpp
enum ConditionCode : U8
{
    CONDITION_CODE_NO_ERROR                  = 0,
    CONDITION_CODE_POS_ACK_LIMIT_REACHED     = 1,
    CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED  = 2,
    CONDITION_CODE_INVALID_TRANSMISSION_MODE = 3,
    CONDITION_CODE_FILESTORE_REJECTION       = 4,
    CONDITION_CODE_FILE_CHECKSUM_FAILURE     = 5,
    CONDITION_CODE_FILE_SIZE_ERROR           = 6,
    CONDITION_CODE_NAK_LIMIT_REACHED         = 7,
    CONDITION_CODE_INACTIVITY_DETECTED       = 8,
    CONDITION_CODE_INVALID_FILE_STRUCTURE    = 9,
    CONDITION_CODE_CHECK_LIMIT_REACHED       = 10,
    CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CONDITION_CODE_SUSPEND_REQUEST_RECEIVED  = 14,
    CONDITION_CODE_CANCEL_REQUEST_RECEIVED   = 15,
};
#endif

/**
 * @brief Structure representing CFDP End of file PDU
 *
 * Defined per section 5.2.2 / table 5-6 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/EofPdu.hpp class implementation
struct PduEof
{
    U8Type  cc;
    U32Type crc;
    U32Type size;
};
#endif

/**
 * @brief Structure representing CFDP Finished PDU
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/FinPdu.hpp class implementation
struct PduFin
{
    U8Type flags;
};
#endif

/**
 * @brief Structure representing CFDP Acknowledge PDU
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
#if 0  // Replaced by Types/AckPdu.hpp class implementation
struct PduAck
{
    U8Type directive_and_subtype_code;
    U8Type cc_and_transaction_status;
};
#endif

/**
 * @brief Structure representing CFDP Segment Request
 *
 * Defined per section 5.2.6 / table 5-11 of CCSDS 727.0-B-5
 */
// Forward declaration - implementation in Types/NakPdu.hpp
struct SegmentRequest;

/**
 * @brief Structure representing CFDP Non-Acknowledge PDU
 *
 * Defined per section 5.2.6 / table 5-10 of CCSDS 727.0-B-5
 */
// Forward declaration - implementation in Types/NakPdu.hpp
struct PduNak;

/**
 * @brief Structure representing CFDP Metadata PDU
 *
 * Defined per section 5.2.5 / table 5-9 of CCSDS 727.0-B-5
 */
// Forward declaration - implementation in Types/MetadataPdu.hpp
struct PduMd;

/**
 * @brief PDU file data header
 */
// Forward declaration - implementation in Types/FileDataPdu.hpp
struct PduFileDataHeader;

/**
 * @brief
 * PDU file data content typedef for limit checking outgoing_file_chunk_size
 * table value and set parameter command.
 *
 * This definition allows for the largest data block possible, as CFDP_MAX_PDU_SIZE -
 * the minimum possible header size.  In practice the outgoing file chunk size is limited by
 * whichever is smaller; the remaining data, remaining space in the packet, and outgoing_file_chunk_size.
 */
// Forward declaration - implementation in Types/FileDataPdu.hpp
struct PduFileDataContent;

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_PDU_HPP */
