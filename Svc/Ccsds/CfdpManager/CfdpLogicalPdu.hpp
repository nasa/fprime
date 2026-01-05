// ======================================================================
// \title  CfdpLogicalPdu.hpp
// \brief  CFDP Logical PDU type definitions
//
// This file is a port of the cf_logical_pdu.hpp file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
// 
// Structures defining logical CFDP PDUs
//
// These are data structures that reflect the logical
// content of the CFDP PDUs defined in CfdpPdu.hpp. Note these are
// _NOT_ intended to reflect the bitwise structures defined
// in the CCSDS blue book, but rather the values contained
// within those structures, in a form that can be used by software.
//
// Specifically, this intent differs in the following ways:
//   - All numeric fields are in native byte order
//   - All structures are padded/aligned according to native CPU (i.e. not packed)
//   - All bit-fields are exploded, where each field/group is a separate member
//   - Variable-size content is normalized, allocated as the maximum possible size
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

/**
 * @file
 *

 */

#ifndef CFDP_LOGICAL_PDU_HPP
#define CFDP_LOGICAL_PDU_HPP

#include "default_cf_extern_typedefs.hpp"
#include "CfdpPdu.hpp"
#include "Svc/Ccsds/CfdpManager/CfdpEntityIdAliasAc.hpp"
#include "Svc/Ccsds/CfdpManager/CfdpTransactionSeqAliasAc.hpp"

namespace Svc {
namespace Ccsds {

/**
 * @brief Maximum number of TLV values in a single PDU
 *
 * This just serves to set an upper bound on the logical structures, to keep
 * things simple.  The real limit varies depending on the specific PDU type
 * being processed.  This caps the amount of storage memory for the worst
 * case, the actual number present is always part of the run-time state.
 *
 * Without filestore requests, use of TLV is pretty limited.
 *
 */
#define CF_PDU_MAX_TLV (4)

/**
 * @brief Maximum number of segment requests in a single PDU
 *
 * Sets an upper bound on the logical structures for the most possible
 * segment structures in a single PDU.
 */
#define CF_PDU_MAX_SEGMENTS (CF_NAK_MAX_SEGMENTS)

/**
 * @brief Type for logical file size/offset value
 *
 * The CFDP protocol permits use of 64-bit values for file size/offsets
 * Although the CF application only supports 32-bit legacy file size
 * type at this point, the logical structures should use this type in
 * case future support for large files is added.
 */
typedef U32 CF_FileSize_t;

/*
 * Note that by exploding the bit-fields into separate members, this will make the
 * storage much less efficient (in many cases using 8 bits to store only 1 logical bit)
 * but this greatly improves and simplifies the access during processing, avoiding
 * repeated shifts and mask.  Furthermore, it only needs to be stored this way
 * during active processing in the engine, and there is only one engine instance,
 * so the extra memory use here is not that impactful (just a single instance).
 *
 * Even if the code evolves to have a separate engine/task per channel, this is
 * still not a big deal to store fields separately this way.
 *
 * Also note that since the bits are not expected to line up at all, sometimes
 * logical fields might occur in a different order than what is in the CCSDS spec,
 * in order to group items of similar type together.
 */

/**
 * @brief Structure representing base CFDP PDU header
 *
 * Reflects the common content at the beginning of all CFDP PDUs, of all types.
 *
 * @sa CF_CFDP_PduHeader_t for encoded form
 */
typedef struct CF_Logical_PduHeader
{
    U8 version;    /**< \brief Version of the protocol */
    U8 pdu_type;   /**< \brief File Directive (0) or File Data (1) */
    U8 direction;  /**< \brief Toward Receiver (0) or Toward Sender (1) */
    U8 txm_mode;   /**< \brief Acknowledged (0) or Unacknowledged (1) */
    U8 crc_flag;   /**< \brief CRC not present (0) or CRC present (1) */
    U8 large_flag; /**< \brief Small/32-bit size (0) or Large/64-bit size (1) */

    U8 segmentation_control; /**< \brief Record boundaries not preserved (0) or preserved (1) */
    U8 eid_length;           /**< \brief Length of encoded entity IDs, in octets (NOT size of logical value) */
    U8 segment_meta_flag;    /**< \brief Segment Metatdata not present (0) or Present (1) */
    U8 txn_seq_length;       /**< \brief Length of encoded sequence number, in octets (NOT size of logical value) */

    U16 header_encoded_length; /**< \brief Length of the encoded PDU header, in octets (NOT sizeof struct) */
    U16 data_encoded_length;   /**< \brief Length of the encoded PDU data, in octets */

    CfdpEntityId       source_eid;      /**< \brief Source entity ID (normalized) */
    CfdpEntityId       destination_eid; /**< \brief Destination entity ID (normalized) */
    CfdpTransactionSeq sequence_num;    /**< \brief Sequence number (normalized) */
} CF_Logical_PduHeader_t;

/**
 * @brief Structure representing logical File Directive header
 *
 * This contains the file directive code from the PDUs for which it applies.
 * The codes are mapped directly to the CFDP protocol values, but converted
 * to a native value (enum) for direct use by software.
 */
typedef struct CF_Logical_PduFileDirectiveHeader
{
    CF_CFDP_FileDirective_t directive_code;
} CF_Logical_PduFileDirectiveHeader_t;

/**
 * @brief Structure representing logical LV Object format
 *
 * These Length + Value pairs used in several CFDP PDU types,
 * typically for storage of strings such as file names.
 *
 * These are only used for string data (mostly filenames) so
 * the data can refer directly to the encoded bits, it does
 * not necessarily need to be duplicated here.
 */
typedef struct CF_Logical_Lv
{
    U8       length;   /**< \brief Length of data field */
    const void *data_ptr; /**< \brief Source of actual data in original location */
} CF_Logical_Lv_t;

/**
 * @brief Union of various data items that may occur in a TLV item
 *
 * The actual type is identified by the "type" field in the enclosing TLV
 *
 * Currently filestore requests are not implemented in CF, so the TLV
 * use is limited.  This may change in the future.
 *
 * Numeric data needs to actually be copied to this buffer, because it needs
 * to be normalized in length and byte-order.  But string data (e.g. filenames,
 * messages) can reside in the original encoded form.
 */
typedef union CF_Logical_TlvData
{
    CfdpEntityId eid;      /**< \brief Valid when type=ENTITY_ID (6) */
    const void *  data_ptr; /**< \brief Source of actual data in original location (other string/binary types) */
} CF_Logical_TlvData_t;

/**
 * @brief Structure representing logical TLV Object format
 *
 * In the current implementation of CF, only entity IDs are
 * currently encoded in this form where indicated in the spec.
 * This may change in a future version.
 *
 * @sa CF_CFDP_tlv_t for encoded form
 */
typedef struct CF_Logical_Tlv
{
    CF_CFDP_TlvType_t    type;   /**< \brief Nature of data field */
    U8                length; /**< \brief Length of data field (encoded length, not local storage size) */
    CF_Logical_TlvData_t data;
} CF_Logical_Tlv_t;

/**
 * @brief Structure representing logical Segment Request data
 */
typedef struct CF_Logical_SegmentRequest
{
    CF_FileSize_t offset_start;
    CF_FileSize_t offset_end;
} CF_Logical_SegmentRequest_t;

typedef struct CF_Logical_SegmentList
{
    U8 num_segments; /**< \brief number of valid entries in the segment list */

    /**
     * \brief Set of all segment requests in this PDU.
     *
     * Number of valid entries is indicated by num_segments,
     * and may be 0 if the PDU does not contain any such fields.
     */
    CF_Logical_SegmentRequest_t segments[CF_PDU_MAX_SEGMENTS];
} CF_Logical_SegmentList_t;

typedef struct CF_Logical_TlvList
{
    U8 num_tlv; /**< \brief number of valid entries in the TLV list */

    CF_Logical_Tlv_t tlv[CF_PDU_MAX_TLV];
} CF_Logical_TlvList_t;

/**
 * @brief Structure representing logical End of file PDU
 *
 * @sa CF_CFDP_PduEof_t for encoded form
 */
typedef struct CF_Logical_PduEof
{
    CF_CFDP_ConditionCode_t cc;
    U32                  crc;
    CF_FileSize_t           size;

    /**
     * \brief Set of all TLV blobs in this PDU.
     */
    CF_Logical_TlvList_t tlv_list;
} CF_Logical_PduEof_t;

/**
 * @brief Structure representing logical Finished PDU
 *
 * @sa CF_CFDP_PduFin_t for encoded form
 */
typedef struct CF_Logical_PduFin
{
    CF_CFDP_ConditionCode_t cc;
    CF_CFDP_FinFileStatus_t file_status;
    U8                   delivery_code; /**< \brief complete file indicated by '0'.  Nonzero means incomplete. */

    /**
     * \brief Set of all TLV blobs in this PDU.
     */
    CF_Logical_TlvList_t tlv_list;
} CF_Logical_PduFin_t;

/**
 * @brief Structure representing CFDP Acknowledge PDU
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
typedef struct CF_Logical_PduAck
{
    U8                   ack_directive_code; /**< \brief directive code of the PDU being ACK'ed */
    U8                   ack_subtype_code;   /**< \brief depends on ack_directive_code  */
    CF_CFDP_ConditionCode_t cc;
    CF_CFDP_AckTxnStatus_t  txn_status;
} CF_Logical_PduAck_t;

/**
 * @brief Structure representing CFDP Metadata PDU
 *
 * Defined per section 5.2.5 / table 5-9 of CCSDS 727.0-B-5
 */
typedef struct CF_Logical_PduMd
{
    U8 close_req;     /**< \brief transaction closure not requested (0) or requested (1) */
    U8 checksum_type; /**< \brief 0 indicates legacy modular checksum */

    CF_FileSize_t size;

    CF_Logical_Lv_t source_filename;
    CF_Logical_Lv_t dest_filename;
} CF_Logical_PduMd_t;

/**
 * @brief Structure representing logical Non-Acknowledge PDU
 */
typedef struct CF_Logical_PduNak
{
    CF_FileSize_t scope_start;
    CF_FileSize_t scope_end;

    /**
     * \brief Set of all segments in this PDU.
     */
    CF_Logical_SegmentList_t segment_list;
} CF_Logical_PduNak_t;

typedef struct CF_Logical_PduFileDataHeader
{
    U8 continuation_state;

    /**
     * \brief the segment_meta_length value will be stored in the
     * segment_list.num_segments field below
     */
    CF_Logical_SegmentList_t segment_list;

    CF_FileSize_t offset; /**< \brief Offset of data in file */

    const void *data_ptr; /**< \brief pointer to read-only data blob within encoded PDU */
    size_t      data_len; /**< \brief Length of data blob within encoded PDU (derived field) */
} CF_Logical_PduFileDataHeader_t;

/**
 * @brief A union of all possible internal header types in a PDU
 *
 * The specific entry which applies depends on the combination of
 * pdu type and directive code.
 */
typedef union CF_Logical_IntHeader
{
    CF_Logical_PduEof_t            eof; /**< \brief valid when pdu_type=0 + directive_code=EOF (4) */
    CF_Logical_PduFin_t            fin; /**< \brief valid when pdu_type=0 + directive_code=FIN (5) */
    CF_Logical_PduAck_t            ack; /**< \brief valid when pdu_type=0 + directive_code=ACK (6) */
    CF_Logical_PduMd_t             md;  /**< \brief valid when pdu_type=0 + directive_code=METADATA (7) */
    CF_Logical_PduNak_t            nak; /**< \brief valid when pdu_type=0 + directive_code=NAK (8) */
    CF_Logical_PduFileDataHeader_t fd;  /**< \brief valid when pdu_type=1 (directive_code is not applicable) */
} CF_Logical_IntHeader_t;

/**
 * @brief Encapsulates the entire PDU information
 *
 */
typedef struct CF_Logical_PduBuffer
{
    /*
     * The encode/decode object tracks the position within the network (encoded) buffer
     * during the encode/decode process.  Only one or the other should be set at
     * a given time, depending on whether this is a received or transmitted PDU.
     */
    struct CF_EncoderState *penc;
    struct CF_DecoderState *pdec;

    /**
     * \brief Data in PDU header is applicable to all packets
     */
    CF_Logical_PduHeader_t pdu_header;

    /**
     * \brief The directive code applies to file directive PDUs, where
     * the pdu_type in the common header is 0. Otherwise this value
     * should be set to 0 for data PDUs (which is a reserved value and
     * does not alias any valid directive code).
     */
    CF_Logical_PduFileDirectiveHeader_t fdirective;

    /**
     * \brief The internal header is specific to the type of PDU being
     * processed. This is a union of all those possible types.
     * See the union definition for which member applies to
     * a given processing cycle.
     */
    CF_Logical_IntHeader_t int_header;

    /**
     * \brief Some PDU types might have a CRC at the end.  If so, this
     * field reflects the value of that CRC.  Its presence/validity
     * depends on the pdu_type and crc_flag in the pdu_header.
     *
     * Note that all CFDP CRCs are 32 bits in length, the blue book
     * does not permit for any other size.
     */
    U32 content_crc;

    // TODO this is a temprorary workaround for buffer allocation
    // Remove this
    U32 index;

} CF_Logical_PduBuffer_t;

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_LOGICAL_PDU_HPP */
