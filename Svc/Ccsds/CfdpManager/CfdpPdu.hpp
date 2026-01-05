// ======================================================================
// \title  CfdpPdu.hpp
// \brief  Structures defining CFDP PDUs
//
// This file is a port of the cf_cfdp_pdu.hpp file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// The structures and enumerations defined in this file with a CF_CFDP
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

#include <Fw/FPrimeBasicTypes.hpp>

#include "default_cf_interface_cfg.hpp"

namespace Svc {
namespace Ccsds {

/**
 * @brief Maximum encoded size of a CFDP PDU header
 *
 * Per the blue book, the size of the Entity ID and Sequence Number may be up to 8 bytes.
 * CF is configurable in what it can accept and transmit, which may be smaller than what
 * the blue book permits.
 */
#define CF_CFDP_MAX_HEADER_SIZE \
    (sizeof(CF_CFDP_PduHeader_t) + (3 * sizeof(CF_CFDP_U64_t))) /* 8 bytes for each variable item */

/**
 * @brief Minimum encoded size of a CFDP PDU header
 *
 * Per the blue book, the size of the Entity ID and Sequence Number must be at least 1 byte.
 */
#define CF_CFDP_MIN_HEADER_SIZE \
    (sizeof(CF_CFDP_PduHeader_t) + (3 * sizeof(CF_CFDP_U8_t))) /* 1 byte for each variable item */

/**
 * @brief Maximum encoded size of a CFDP PDU that this implementation can accept
 *
 * This definition reflects the current configuration of the CF application.
 * Note that this is based on the size of the native representation of Entity ID and
 * sequence number.  Although the bitwise representations of these items are
 * different in the encoded packets vs. the native representation, the basic size
 * still correlates (e.g. if it takes 4 bytes natively, it will be encoded into
 * 4 bytes).
 */
#define CF_APP_MAX_HEADER_SIZE (sizeof(CF_CFDP_PduHeader_t) + sizeof(CfdpTransactionSeq) + (3 * sizeof(CfdpEntityId)))

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
typedef struct
{
    U8 octets[1];
} CF_CFDP_U8_t;

/**
 * @brief Encoded 16-bit value in the CFDP PDU
 */
typedef struct
{
    U8 octets[2];
} CF_CFDP_U16_t;

/**
 * @brief Encoded 32-bit value in the CFDP PDU
 */
typedef struct
{
    U8 octets[4];
} CF_CFDP_U32_t;

/**
 * @brief Encoded 64-bit value in the CFDP PDU
 */
typedef struct
{
    U8 octets[8];
} CF_CFDP_U64_t;

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
 * in this definition.  As a result, the sizeof(CF_CFDP_PduHeader_t) reflects only the
 * size of the fixed fields.  Use CF_HeaderSize() to get the actual size of this structure.
 */
typedef struct CF_CFDP_PduHeader
{
    CF_CFDP_U8_t  flags;           /**< \brief Flags indicating the PDU type, direction, mode, etc */
    CF_CFDP_U16_t length;          /**< \brief Length of the entire PDU, in octets */
    CF_CFDP_U8_t  eid_tsn_lengths; /**< \brief Lengths of the EID+TSN data (bitfields) */

    /* variable-length data goes here - it is at least 3 additional bytes */
} CF_CFDP_PduHeader_t;

/**
 * @brief Structure representing CFDP File Directive Header
 *
 * Defined per section 5.2 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduFileDirectiveHeader
{
    CF_CFDP_U8_t directive_code;
} CF_CFDP_PduFileDirectiveHeader_t;

/**
 * @brief Structure representing CFDP LV Object format
 *
 * These Length + Value pairs used in several CFDP PDU types,
 * typically for storage of strings such as file names.
 *
 * Defined per table 5-2 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_lv
{
    CF_CFDP_U8_t length; /**< \brief Length of data field */
} CF_CFDP_lv_t;

/**
 * @brief Structure representing CFDP TLV Object format
 *
 * These Type + Length + Value pairs used in several CFDP PDU types,
 * typically for file storage requests (section 5.4).
 *
 * Defined per table 5-3 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_tlv
{
    CF_CFDP_U8_t type;   /**< \brief Nature of data field */
    CF_CFDP_U8_t length; /**< \brief Length of data field */
} CF_CFDP_tlv_t;

/**
 * @brief Values for "type" field of TLV structure
 *
 * Defined per section 5.4 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_TLV_TYPE_FILESTORE_REQUEST      = 0,
    CF_CFDP_TLV_TYPE_FILESTORE_RESPONSE     = 1,
    CF_CFDP_TLV_TYPE_MESSAGE_TO_USER        = 2,
    CF_CFDP_TLV_TYPE_FAULT_HANDLER_OVERRIDE = 4,
    CF_CFDP_TLV_TYPE_FLOW_LABEL             = 5,
    CF_CFDP_TLV_TYPE_ENTITY_ID              = 6,
    CF_CFDP_TLV_TYPE_INVALID_MAX            = 7
} CF_CFDP_TlvType_t;

/**
 * @brief Values for "directive_code" within CF_CFDP_PduFileDirectiveHeader_t
 *
 * Defined per table 5-4 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FileDirective_INVALID_MIN = 0, /**< \brief Minimum used to limit range */
    CF_CFDP_FileDirective_EOF         = 4,
    CF_CFDP_FileDirective_FIN         = 5,
    CF_CFDP_FileDirective_ACK         = 6,
    CF_CFDP_FileDirective_METADATA    = 7,
    CF_CFDP_FileDirective_NAK         = 8,
    CF_CFDP_FileDirective_PROMPT      = 9,
    CF_CFDP_FileDirective_KEEP_ALIVE  = 12,
    CF_CFDP_FileDirective_INVALID_MAX = 13, /**< \brief Maximum used to limit range */
} CF_CFDP_FileDirective_t;

/**
 * @brief Values for "acknowledgment transfer status"
 *
 * This enum is pertinent to the ACK PDU type, defines the
 * values for the directive field.
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_AckTxnStatus_UNDEFINED    = 0,
    CF_CFDP_AckTxnStatus_ACTIVE       = 1,
    CF_CFDP_AckTxnStatus_TERMINATED   = 2,
    CF_CFDP_AckTxnStatus_UNRECOGNIZED = 3,
    CF_CFDP_AckTxnStatus_INVALID      = 4,
} CF_CFDP_AckTxnStatus_t;

/**
 * @brief Values for "finished delivery code"
 *
 * This enum is pertinent to the FIN PDU type, defines the
 * values for the delivery code field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FinDeliveryCode_COMPLETE   = 0,
    CF_CFDP_FinDeliveryCode_INCOMPLETE = 1,
    CF_CFDP_FinDeliveryCode_INVALID    = 2,
} CF_CFDP_FinDeliveryCode_t;

/**
 * @brief Values for "finished file status"
 *
 * This enum is pertinent to the FIN PDU type, defines the
 * values for the file status field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FinFileStatus_DISCARDED           = 0,
    CF_CFDP_FinFileStatus_DISCARDED_FILESTORE = 1,
    CF_CFDP_FinFileStatus_RETAINED            = 2,
    CF_CFDP_FinFileStatus_UNREPORTED          = 3,
    CF_CFDP_FinFileStatus_INVALID             = 4,
} CF_CFDP_FinFileStatus_t;

/**
 * @brief Values for "condition code"
 *
 * This enum defines the values for the condition code field
 * for the PDU types which have this field (EOF, FIN, ACK)
 *
 * Defined per table 5-5 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_ConditionCode_NO_ERROR                  = 0,
    CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED     = 1,
    CF_CFDP_ConditionCode_KEEP_ALIVE_LIMIT_REACHED  = 2,
    CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE = 3,
    CF_CFDP_ConditionCode_FILESTORE_REJECTION       = 4,
    CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE     = 5,
    CF_CFDP_ConditionCode_FILE_SIZE_ERROR           = 6,
    CF_CFDP_ConditionCode_NAK_LIMIT_REACHED         = 7,
    CF_CFDP_ConditionCode_INACTIVITY_DETECTED       = 8,
    CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE    = 9,
    CF_CFDP_ConditionCode_CHECK_LIMIT_REACHED       = 10,
    CF_CFDP_ConditionCode_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CF_CFDP_ConditionCode_SUSPEND_REQUEST_RECEIVED  = 14,
    CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED   = 15,
} CF_CFDP_ConditionCode_t;

/**
 * @brief Structure representing CFDP End of file PDU
 *
 * Defined per section 5.2.2 / table 5-6 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduEof
{
    CF_CFDP_U8_t  cc;
    CF_CFDP_U32_t crc;
    CF_CFDP_U32_t size;
} CF_CFDP_PduEof_t;

/**
 * @brief Structure representing CFDP Finished PDU
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduFin
{
    CF_CFDP_U8_t flags;
} CF_CFDP_PduFin_t;

/**
 * @brief Structure representing CFDP Acknowledge PDU
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduAck
{
    CF_CFDP_U8_t directive_and_subtype_code;
    CF_CFDP_U8_t cc_and_transaction_status;
} CF_CFDP_PduAck_t;

/**
 * @brief Structure representing CFDP Segment Request
 *
 * Defined per section 5.2.6 / table 5-11 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_SegmentRequest
{
    CF_CFDP_U32_t offset_start;
    CF_CFDP_U32_t offset_end;
} CF_CFDP_SegmentRequest_t;

/**
 * @brief Structure representing CFDP Non-Acknowledge PDU
 *
 * Defined per section 5.2.6 / table 5-10 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduNak
{
    CF_CFDP_U32_t scope_start;
    CF_CFDP_U32_t scope_end;
} CF_CFDP_PduNak_t;

/**
 * @brief Structure representing CFDP Metadata PDU
 *
 * Defined per section 5.2.5 / table 5-9 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduMd
{
    CF_CFDP_U8_t  segmentation_control;
    CF_CFDP_U32_t size;
} CF_CFDP_PduMd_t;

/**
 * @brief PDU file data header
 */
typedef struct CF_CFDP_PduFileDataHeader
{
    /**
     * NOTE: while this is the only fixed/required field in the data PDU, it may
     * have segment metadata prior to this, depending on how the fields in the
     * base header are set
     */
    CF_CFDP_U32_t offset;
} CF_CFDP_PduFileDataHeader_t;

/**
 * @brief
 * PDU file data content typedef for limit checking outgoing_file_chunk_size
 * table value and set parameter command.
 *
 * This definition allows for the largest data block possible, as CF_MAX_PDU_SIZE -
 * the minimum possible header size.  In practice the outgoing file chunk size is limited by
 * whichever is smaller; the remaining data, remaining space in the packet, and outgoing_file_chunk_size.
 */
typedef struct CF_CFDP_PduFileDataContent
{
    U8 data[CF_MAX_PDU_SIZE - sizeof(CF_CFDP_PduFileDataHeader_t) - CF_CFDP_MIN_HEADER_SIZE];
} CF_CFDP_PduFileDataContent_t;

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_PDU_HPP */
