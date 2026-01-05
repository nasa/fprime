// ======================================================================
// \title  CfdpCodec.cpp
// \brief  CFDP protocol implementation
//
// This file is a port of the cf_codec.cpp file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// CFDP protocol data structure encode/decode implementation
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

#include "CfdpPdu.hpp"
#include "CfdpCodec.hpp"

#include <stdint.h>
#include <string.h>

namespace Svc {
namespace Ccsds {

typedef struct CF_Codec_BitField
{
    U32 shift;
    U32 mask;
} CF_Codec_BitField_t;

/* NBITS == number of bits */
#define CF_INIT_FIELD(NBITS, SHIFT)      \
    {                                    \
        (SHIFT), ((1u << (NBITS)) - 1u)  \
    }

/*
 * All CFDP sub-fields are fewer than 8 bits in size
 */
static inline U8 CF_FieldGetVal(const U8 *src, U8 shift, U8 mask)
{
    return (*src >> shift) & mask;
}

static inline void CF_FieldSetVal(U8 *dest, U8 shift, U8 mask, U8 val)
{
    *dest &= static_cast<U8>(~(mask << shift));
    *dest |= static_cast<U8>((val & mask) << shift);
}

/* FGV, FSV, and FAV are just simple shortenings of the field macros.
 *
 * FGV == field get val
 * FSV == field set val
 */

#define FGV(SRC, NAME)       (CF_FieldGetVal(static_cast<const U8 *>((SRC).octets), \
                                             static_cast<U8>((NAME).shift), \
                                             static_cast<U8>((NAME).mask)))
#define FSV(DEST, NAME, VAL) (CF_FieldSetVal(static_cast<U8 *>((DEST).octets), \
                                             static_cast<U8>((NAME).shift), \
                                             static_cast<U8>((NAME).mask), \
                                             static_cast<U8>(VAL)))

/*
 * Fields within the "flags" byte of the PDU header
 */
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_VERSION   = CF_INIT_FIELD(3, 5);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_TYPE      = CF_INIT_FIELD(1, 4);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_DIR       = CF_INIT_FIELD(1, 3);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_MODE      = CF_INIT_FIELD(1, 2);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_CRC       = CF_INIT_FIELD(1, 1);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_FLAGS_LARGEFILE = CF_INIT_FIELD(1, 0);

/*
 * Fields within the "eid_tsn_lengths" byte of the PDU header
 */
static const CF_Codec_BitField_t CF_CFDP_PduHeader_SEGMENTATION_CONTROL         = CF_INIT_FIELD(1, 7);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_LENGTHS_ENTITY               = CF_INIT_FIELD(3, 4);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_SEGMENT_METADATA             = CF_INIT_FIELD(1, 3);
static const CF_Codec_BitField_t CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE = CF_INIT_FIELD(3, 0);

/*
 * Position of the condition code value within the CC field for EOF
 */
static const CF_Codec_BitField_t CF_CFDP_PduEof_FLAGS_CC = CF_INIT_FIELD(4, 4);

/*
 * Position of the sub-field values within the flags field for FIN
 */
static const CF_Codec_BitField_t CF_CFDP_PduFin_FLAGS_CC            = CF_INIT_FIELD(4, 4);
static const CF_Codec_BitField_t CF_CFDP_PduFin_FLAGS_DELIVERY_CODE = CF_INIT_FIELD(1, 2);
static const CF_Codec_BitField_t CF_CFDP_PduFin_FLAGS_FILE_STATUS   = CF_INIT_FIELD(2, 0);

/*
 * Position of the sub-field values within the directive_and_subtype_code
 * and cc_and_transaction_status fields within the ACK PDU.
 */
static const CF_Codec_BitField_t CF_CFDP_PduAck_DIR_CODE           = CF_INIT_FIELD(4, 4);
static const CF_Codec_BitField_t CF_CFDP_PduAck_DIR_SUBTYPE_CODE   = CF_INIT_FIELD(4, 0);
static const CF_Codec_BitField_t CF_CFDP_PduAck_CC                 = CF_INIT_FIELD(4, 4);
static const CF_Codec_BitField_t CF_CFDP_PduAck_TRANSACTION_STATUS = CF_INIT_FIELD(2, 0);

/*
 * Position of the sub-field values within the directive_and_subtype_code
 * and cc_and_transaction_status fields within the ACK PDU.
 */
static const CF_Codec_BitField_t CF_CFDP_PduMd_CLOSURE_REQUESTED = CF_INIT_FIELD(1, 7);
static const CF_Codec_BitField_t CF_CFDP_PduMd_CHECKSUM_TYPE     = CF_INIT_FIELD(4, 0);

/*
 * Position of the optional sub-field values within the file data PDU header
 * These are present only if the "segment metadata" flag in the common header
 * is set to 1.
 */
static const CF_Codec_BitField_t CF_CFDP_PduFileData_RECORD_CONTINUATION_STATE = CF_INIT_FIELD(2, 6);
static const CF_Codec_BitField_t CF_CFDP_PduFileData_SEGMENT_METADATA_LENGTH   = CF_INIT_FIELD(6, 0);

/* NOTE: get/set will handle endianness */
/*
 * ALSO NOTE: These store/set inline functions/macros are used with
 * literal integers as well as variables.  So they operate on value, where
 * the load/get functions operate by reference
 */

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Store_U8(CF_CFDP_U8_t *pdst, U8 val)
{
    pdst->octets[0] = val;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Store_U16(CF_CFDP_U16_t *pdst, U16 val)
{
    pdst->octets[1] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[0] = static_cast<U8>(val & 0xFF);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Store_U32(CF_CFDP_U32_t *pdst, U32 val)
{
    pdst->octets[3] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[2] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[1] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[0] = static_cast<U8>(val & 0xFF);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Store_U64(CF_CFDP_U64_t *pdst, U64 val)
{
    pdst->octets[7] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[6] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[5] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[4] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[3] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[2] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[1] = static_cast<U8>(val & 0xFF);
    val >>= 8;
    pdst->octets[0] = static_cast<U8>(val & 0xFF);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Load_U8(U8 *pdst, const CF_CFDP_U8_t *psrc)
{
    *pdst = psrc->octets[0];
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Load_U16(U16 *pdst, const CF_CFDP_U16_t *psrc)
{
    U16 val = 0;

    val |= psrc->octets[0];
    val = static_cast<U16>(val << 8);
    val |= psrc->octets[1];

    *pdst = val;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Load_U32(U32 *pdst, const CF_CFDP_U32_t *psrc)
{
    U32 val = 0;

    val |= psrc->octets[0];
    val = static_cast<U32>(val << 8);
    val |= psrc->octets[1];
    val = static_cast<U32>(val << 8);
    val |= psrc->octets[2];
    val = static_cast<U32>(val << 8);
    val |= psrc->octets[3];

    *pdst = val;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_Codec_Load_U64(U64 *pdst, const CF_CFDP_U64_t *psrc)
{
    U64 val = 0;

    val |= psrc->octets[0];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[1];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[2];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[3];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[4];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[5];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[6];
    val = static_cast<U64>(val << 8);
    val |= psrc->octets[7];

    *pdst = val;
}

bool CF_CFDP_CodecCheckSize(CF_CodecState_t *state, size_t chunksize)
{
    size_t next_offset = state->next_offset + chunksize;

    if (next_offset > state->max_size)
    {
        CF_CFDP_CodecSetDone(state);
    }
    else
    {
        state->next_offset = next_offset;
    }

    return CF_CFDP_CodecIsOK(state);
}

void *CF_CFDP_DoEncodeChunk(CF_EncoderState_t *state, size_t chunksize)
{
    U8 *buf = state->base + CF_CFDP_CodecGetPosition(&state->codec_state);

    if (!CF_CFDP_CodecCheckSize(&state->codec_state, chunksize))
    {
        buf = NULL;
    }

    return buf;
}

const void *CF_CFDP_DoDecodeChunk(CF_DecoderState_t *state, size_t chunksize)
{
    const U8 *buf = state->base + CF_CFDP_CodecGetPosition(&state->codec_state);

    if (!CF_CFDP_CodecCheckSize(&state->codec_state, chunksize))
    {
        buf = NULL;
    }

    return buf;
}

U8 CF_CFDP_GetValueEncodedSize(U64 Value)
{
    U8  MinSize;
    U64 Limit = 0x100;

    for (MinSize = 1; MinSize < 8 && Value >= Limit; ++MinSize)
    {
        Limit <<= 8;
    }

    return MinSize;
}

void CF_EncodeIntegerInSize(CF_EncoderState_t *state, U64 value, U8 encode_size)
{
    U8 *dptr;

    dptr = static_cast<U8*>(CF_CFDP_DoEncodeChunk(state, encode_size));
    if (dptr != NULL)
    {
        /* this writes from LSB to MSB, in reverse (so the result will be in network order) */
        dptr += encode_size;
        while (encode_size > 0)
        {
            --encode_size;
            --dptr;
            *dptr = value & 0xFF;
            value >>= 8;
        }
    }
}

void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    CF_CFDP_PduHeader_t *peh; /* for encoding fixed sized fields */

    peh = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduHeader_t);
    if (peh != NULL)
    {
        CF_Codec_Store_U8(&(peh->flags), 0);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_VERSION, plh->version);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_DIR, plh->direction);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_TYPE, plh->pdu_type);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_MODE, plh->txm_mode);

        /* The eid+tsn lengths are encoded as -1 */
        CF_Codec_Store_U8(&(peh->eid_tsn_lengths), 0);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_SEGMENTATION_CONTROL, plh->segmentation_control);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_ENTITY, plh->eid_length - 1);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_SEGMENT_METADATA, plh->segment_meta_flag);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE, plh->txn_seq_length - 1);

        /* NOTE: peh->length is NOT set here, as it depends on future encoding */

        /* Now copy variable-length fields */
        CF_EncodeIntegerInSize(state, plh->source_eid, plh->eid_length);
        CF_EncodeIntegerInSize(state, plh->sequence_num, plh->txn_seq_length);
        CF_EncodeIntegerInSize(state, plh->destination_eid, plh->eid_length);

        /* The position now reflects the length of the basic header */
        plh->header_encoded_length = static_cast<U16>(CF_CODEC_GET_POSITION(state));
    }
}

void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    CF_CFDP_PduHeader_t *peh;

    /*
     * This is different as it is updating a block that was already encoded,
     * so it cannot use CF_ENCODE_FIXED_CHUNK because this adds an entity to the tail.
     *
     * The PDU header that needs update is the very first entity in the packet, and
     * this should never be NULL.
     */
    if (CF_CODEC_IS_OK(state) && CF_CODEC_GET_POSITION(state) >= sizeof(CF_CFDP_PduHeader_t))
    {
        peh = reinterpret_cast<CF_CFDP_PduHeader_t *>(state->base);

        /* Total length is a simple 16-bit quantity */
        CF_Codec_Store_U16(&(peh->length), plh->data_encoded_length);
    }

    /* This "closes" the packet so nothing else can be added to this EncoderState,
     * it is not indicative of an error */
    CF_CODEC_SET_DONE(state);
}

void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    CF_CFDP_PduFileDirectiveHeader_t *peh; /* for encoding fixed sized fields */
    U8                             value = pfdir->directive_code;

    peh = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFileDirectiveHeader_t);
    if (peh != NULL)
    {
        CF_Codec_Store_U8(&(peh->directive_code), value);
    }
}

void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv)
{
    CF_CFDP_lv_t *lv; /* for encoding fixed sized fields */
    void *        data_ptr;

    lv = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_lv_t);
    if (lv != NULL)
    {
        CF_Codec_Store_U8(&(lv->length), pllv->length);
        if (pllv->length > 0)
        {
            data_ptr = CF_CFDP_DoEncodeChunk(state, pllv->length);
            if (data_ptr != NULL && pllv->data_ptr != NULL)
            {
                memcpy(data_ptr, pllv->data_ptr, pllv->length);
            }
            else
            {
                CF_CODEC_SET_DONE(state);
            }
        }
    }
}

void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    CF_CFDP_tlv_t *tlv; /* for encoding fixed sized fields */
    void *         data_ptr;

    tlv = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_tlv_t);
    if (tlv != NULL)
    {
        CF_Codec_Store_U8(&(tlv->type), pltlv->type);
        CF_Codec_Store_U8(&(tlv->length), pltlv->length);

        /* the only TLV type currently implemented is entity id */
        if (pltlv->type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            CF_EncodeIntegerInSize(state, pltlv->data.eid, pltlv->length);
        }
        else if (pltlv->length > 0)
        {
            /* Copy the other data in (feature not used in CF yet, but should be handled) */
            data_ptr = CF_CFDP_DoEncodeChunk(state, pltlv->length);
            if (data_ptr != NULL && pltlv->data.data_ptr != NULL)
            {
                memcpy(data_ptr, pltlv->data.data_ptr, pltlv->length);
            }
            else
            {
                CF_CODEC_SET_DONE(state);
            }
        }
    }
}

void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    CF_CFDP_SegmentRequest_t *sr; /* for encoding fixed sized fields */

    sr = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_SegmentRequest_t);
    if (sr != NULL)
    {
        CF_Codec_Store_U32(&(sr->offset_start), plseg->offset_start);
        CF_Codec_Store_U32(&(sr->offset_end), plseg->offset_end);
    }
}

void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv)
{
    U8 i;

    for (i = 0; CF_CODEC_IS_OK(state) && i < pltlv->num_tlv; ++i)
    {
        CF_CFDP_EncodeTLV(state, &pltlv->tlv[i]);
    }
}

void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg)
{
    U8 i;

    for (i = 0; CF_CODEC_IS_OK(state) && i < plseg->num_segments; ++i)
    {
        CF_CFDP_EncodeSegmentRequest(state, &plseg->segments[i]);
    }
}

void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    CF_CFDP_PduMd_t *md; /* for encoding fixed sized fields */

    md = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduMd_t);
    if (md != NULL)
    {
        CF_Codec_Store_U8(&(md->segmentation_control), 0);
        FSV(md->segmentation_control, CF_CFDP_PduMd_CLOSURE_REQUESTED, plmd->close_req);
        FSV(md->segmentation_control, CF_CFDP_PduMd_CHECKSUM_TYPE, plmd->checksum_type);
        CF_Codec_Store_U32(&(md->size), plmd->size);

        /* Add in LV for src/dest */
        CF_CFDP_EncodeLV(state, &plmd->source_filename);
        CF_CFDP_EncodeLV(state, &plmd->dest_filename);
    }
}

void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    CF_CFDP_PduFileDataHeader_t *fd;
    CF_CFDP_U8_t *            optional_fields;

    /* in this packet, the optional fields actually come first */
    if (with_meta)
    {
        optional_fields = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_U8_t);
    }
    else
    {
        optional_fields = NULL;
    }

    if (optional_fields != NULL)
    {
        CF_Codec_Store_U8(optional_fields, 0);
        FSV(*optional_fields, CF_CFDP_PduFileData_RECORD_CONTINUATION_STATE, plfd->continuation_state);
        FSV(*optional_fields, CF_CFDP_PduFileData_SEGMENT_METADATA_LENGTH, plfd->segment_list.num_segments);

        CF_CFDP_EncodeAllSegments(state, &plfd->segment_list);
    }

    fd = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFileDataHeader_t);
    if (fd != NULL)
    {
        CF_Codec_Store_U32(&(fd->offset), plfd->offset);
    }
}

void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    CF_CFDP_PduEof_t *eof; /* for encoding fixed sized fields */

    eof = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduEof_t);
    if (eof != NULL)
    {
        CF_Codec_Store_U8(&(eof->cc), 0);
        FSV(eof->cc, CF_CFDP_PduEof_FLAGS_CC, pleof->cc);
        CF_Codec_Store_U32(&(eof->crc), pleof->crc);
        CF_Codec_Store_U32(&(eof->size), pleof->size);

        CF_CFDP_EncodeAllTlv(state, &pleof->tlv_list);
    }
}

void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    CF_CFDP_PduFin_t *fin; /* for encoding fixed sized fields */

    fin = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFin_t);
    if (fin != NULL)
    {
        CF_Codec_Store_U8(&(fin->flags), 0);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_CC, plfin->cc);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_DELIVERY_CODE, plfin->delivery_code);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_FILE_STATUS, plfin->file_status);

        CF_CFDP_EncodeAllTlv(state, &plfin->tlv_list);
    }
}

void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack)
{
    CF_CFDP_PduAck_t *ack; /* for encoding fixed sized fields */

    ack = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduAck_t);
    if (ack != NULL)
    {
        CF_Codec_Store_U8(&(ack->directive_and_subtype_code), 0);
        FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_CODE, plack->ack_directive_code);
        FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_SUBTYPE_CODE, plack->ack_subtype_code);

        CF_Codec_Store_U8(&(ack->cc_and_transaction_status), 0);
        FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_CC, plack->cc);
        FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_TRANSACTION_STATUS, plack->txn_status);
    }
}

void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    CF_CFDP_PduNak_t *nak; /* for encoding fixed sized fields */

    nak = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduNak_t);
    if (nak != NULL)
    {
        CF_Codec_Store_U32(&(nak->scope_start), plnak->scope_start);
        CF_Codec_Store_U32(&(nak->scope_end), plnak->scope_end);

        CF_CFDP_EncodeAllSegments(state, &plnak->segment_list);
    }
}

void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, U32 *plcrc)
{
    CF_CFDP_U32_t *pecrc; /* CFDP CRC values are 32-bit only, per blue book */

    pecrc = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_U32_t);
    if (pecrc != NULL)
    {
        CF_Codec_Store_U32(pecrc, *plcrc);
    }
}

U64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, U8 decode_size)
{
    const U8 *sptr;
    U64       temp_val;

    temp_val = 0;
    sptr     = static_cast<const U8 *>(CF_CFDP_DoDecodeChunk(state, decode_size));
    if (sptr != NULL)
    {
        /* this reads from MSB to LSB, so the result will be in native order */
        while (decode_size > 0)
        {
            temp_val = static_cast<U64>(temp_val << 8);
            temp_val |= *sptr & 0xFF;
            ++sptr;
            --decode_size;
        }
    }

    return temp_val;
}

bool CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    const CF_CFDP_PduHeader_t *peh; /* for decoding fixed sized fields */
    bool                       ret = true;

    /* decode the standard PDU header */
    peh = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduHeader_t);
    if (peh != NULL)
    {
        plh->version    = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_VERSION);
        plh->direction  = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_DIR);
        plh->pdu_type   = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_TYPE);
        plh->txm_mode   = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_MODE);
        plh->crc_flag   = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_CRC);
        plh->large_flag = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_LARGEFILE);

        /* The eid+tsn lengths are encoded as -1 */
        plh->segmentation_control = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_SEGMENTATION_CONTROL);
        plh->eid_length           = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_ENTITY) + 1;
        plh->segment_meta_flag    = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_SEGMENT_METADATA);
        plh->txn_seq_length       = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE) + 1;

        /* Length is a simple 16-bit quantity and refers to the content after this header */
        CF_Codec_Load_U16(&(plh->data_encoded_length), &(peh->length));
        if ((plh->eid_length > sizeof(plh->source_eid)) || (plh->txn_seq_length > sizeof(plh->sequence_num)))
        {
            ret = false;
        }
        else
        {
            /* Now copy variable-length fields */
            plh->source_eid      = static_cast<U32>(CF_DecodeIntegerInSize(state, plh->eid_length));
            plh->sequence_num    = static_cast<U32>(CF_DecodeIntegerInSize(state, plh->txn_seq_length));
            plh->destination_eid = static_cast<U32>(CF_DecodeIntegerInSize(state, plh->eid_length));

            /* The header length is where decoding ended at this point */
            plh->header_encoded_length = static_cast<U16>(CF_CODEC_GET_POSITION(state));
        }
    }
    return ret;
}

void CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    const CF_CFDP_PduFileDirectiveHeader_t *peh;
    U8                                   packet_val;

    /* decode the standard PDU header */
    peh = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFileDirectiveHeader_t);
    if (peh != NULL)
    {
        CF_Codec_Load_U8(&packet_val, &(peh->directive_code));
        pfdir->directive_code = static_cast<CF_CFDP_FileDirective_t>(packet_val);
    }
}

void CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv)
{
    const CF_CFDP_lv_t *lv;

    lv = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_lv_t);
    if (lv != NULL)
    {
        CF_Codec_Load_U8(&(pllv->length), &(lv->length));
        pllv->data_ptr = CF_CFDP_DoDecodeChunk(state, pllv->length);
    }
}

void CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    const CF_CFDP_tlv_t *tlv;
    U8                type_val;

    tlv = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_tlv_t);
    if (tlv != NULL)
    {
        CF_Codec_Load_U8(&type_val, &(tlv->type));
        CF_Codec_Load_U8(&(pltlv->length), &(tlv->length));

        /* the only TLV type currently implemented is entity id */
        pltlv->type = static_cast<CF_CFDP_TlvType_t>(type_val);
        if (pltlv->type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            pltlv->data.eid = static_cast<U32>(CF_DecodeIntegerInSize(state, pltlv->length));
        }
        else
        {
            /* not implemented, but must not send random data */
            pltlv->data.data_ptr = CF_CFDP_DoDecodeChunk(state, pltlv->length);
        }
    }
}

void CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    const CF_CFDP_SegmentRequest_t *sr; /* for decoding fixed sized fields */

    sr = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_SegmentRequest_t);
    if (sr != NULL)
    {
        CF_Codec_Load_U32(&(plseg->offset_start), &(sr->offset_start));
        CF_Codec_Load_U32(&(plseg->offset_end), &(sr->offset_end));
    }
}

void CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    const CF_CFDP_PduMd_t *md; /* for decoding fixed sized fields */

    md = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduMd_t);
    if (md != NULL)
    {
        plmd->close_req     = FGV(md->segmentation_control, CF_CFDP_PduMd_CLOSURE_REQUESTED);
        plmd->checksum_type = FGV(md->segmentation_control, CF_CFDP_PduMd_CHECKSUM_TYPE);
        CF_Codec_Load_U32(&(plmd->size), &(md->size));

        /* Add in LV for src/dest */
        CF_CFDP_DecodeLV(state, &plmd->source_filename);
        CF_CFDP_DecodeLV(state, &plmd->dest_filename);
    }
}

void CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    const CF_CFDP_PduFileDataHeader_t *fd;
    const CF_CFDP_U8_t *            optional_fields;
    U8                              field_count;

    plfd->continuation_state        = 0;
    plfd->segment_list.num_segments = 0;

    /* in this packet, the optional fields actually come first */
    if (with_meta)
    {
        optional_fields = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_U8_t);
    }
    else
    {
        optional_fields = NULL;
    }

    if (optional_fields != NULL)
    {
        plfd->continuation_state = FGV(*optional_fields, CF_CFDP_PduFileData_RECORD_CONTINUATION_STATE);
        field_count              = FGV(*optional_fields, CF_CFDP_PduFileData_SEGMENT_METADATA_LENGTH);
        if (field_count > CF_PDU_MAX_SEGMENTS)
        {
            /* do not overfill */
            CF_CODEC_SET_DONE(state);
            field_count = 0;
        }

        while (field_count > 0)
        {
            --field_count;

            /* append decoded segment info */
            CF_CFDP_DecodeSegmentRequest(state, &plfd->segment_list.segments[plfd->segment_list.num_segments]);
            if (!CF_CODEC_IS_OK(state))
            {
                break;
            }

            /* only increment if successful */
            ++plfd->segment_list.num_segments;
        }
    }

    fd = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFileDataHeader_t);
    if (fd != NULL)
    {
        CF_Codec_Load_U32(&(plfd->offset), &(fd->offset));

        plfd->data_len = CF_CODEC_GET_REMAIN(state);
        plfd->data_ptr = CF_CFDP_DoDecodeChunk(state, plfd->data_len);
    }
}

void CF_CFDP_DecodeCrc(CF_DecoderState_t *state, U32 *plcrc)
{
    const CF_CFDP_U32_t *pecrc; /* CFDP CRC values are 32-bit only, per blue book */

    pecrc = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_U32_t);
    if (pecrc != NULL)
    {
        CF_Codec_Load_U32(plcrc, pecrc);
    }
}

void CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    const CF_CFDP_PduEof_t *eof; /* for decoding fixed sized fields */

    eof = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduEof_t);
    if (eof != NULL)
    {
        pleof->cc = static_cast<CF_CFDP_ConditionCode_t>(FGV(eof->cc, CF_CFDP_PduEof_FLAGS_CC));
        CF_Codec_Load_U32(&(pleof->crc), &(eof->crc));
        CF_Codec_Load_U32(&(pleof->size), &(eof->size));

        CF_CFDP_DecodeAllTlv(state, &pleof->tlv_list, CF_PDU_MAX_TLV);
    }
}

void CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    const CF_CFDP_PduFin_t *fin; /* for decoding fixed sized fields */

    fin = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFin_t);
    if (fin != NULL)
    {
        plfin->cc            = static_cast<CF_CFDP_ConditionCode_t>(FGV(fin->flags, CF_CFDP_PduFin_FLAGS_CC));
        plfin->delivery_code = FGV(fin->flags, CF_CFDP_PduFin_FLAGS_DELIVERY_CODE);
        plfin->file_status   = static_cast<CF_CFDP_FinFileStatus_t>(FGV(fin->flags, CF_CFDP_PduFin_FLAGS_FILE_STATUS));

        CF_CFDP_DecodeAllTlv(state, &plfin->tlv_list, CF_PDU_MAX_TLV);
    }
}

void CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack)
{
    const CF_CFDP_PduAck_t *ack; /* for decoding fixed sized fields */

    ack = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduAck_t);
    if (ack != NULL)
    {
        plack->ack_directive_code = FGV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_CODE);
        plack->ack_subtype_code   = FGV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_SUBTYPE_CODE);

        plack->cc         = static_cast<CF_CFDP_ConditionCode_t>(FGV(ack->cc_and_transaction_status, CF_CFDP_PduAck_CC));
        plack->txn_status = static_cast<CF_CFDP_AckTxnStatus_t>(FGV(ack->cc_and_transaction_status, CF_CFDP_PduAck_TRANSACTION_STATUS));
    }
}

void CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    const CF_CFDP_PduNak_t *nak; /* for encoding fixed sized fields */

    nak = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduNak_t);
    if (nak != NULL)
    {
        CF_Codec_Load_U32(&(plnak->scope_start), &(nak->scope_start));
        CF_Codec_Load_U32(&(plnak->scope_end), &(nak->scope_end));

        CF_CFDP_DecodeAllSegments(state, &plnak->segment_list, CF_PDU_MAX_SEGMENTS);
    }
}

void CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, U8 limit)
{
    pltlv->num_tlv = 0;

    /* The set of TLV data may exactly consume the rest of the PDU, this is OK */
    while (limit > 0 && CF_CODEC_GET_REMAIN(state) != 0)
    {
        --limit;

        if (pltlv->num_tlv >= CF_PDU_MAX_TLV)
        {
            /* too many */
            CF_CODEC_SET_DONE(state);
        }
        else
        {
            CF_CFDP_DecodeTLV(state, &pltlv->tlv[pltlv->num_tlv]);
        }

        if (!CF_CODEC_IS_OK(state))
        {
            break;
        }

        /* only increment if above was successful */
        ++pltlv->num_tlv;
    }
}

void CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, U8 limit)
{
    plseg->num_segments = 0;

    /* The set of SegmentRequest data may exactly consume the rest of the PDU, this is OK */
    while (limit > 0 && CF_CODEC_GET_REMAIN(state) != 0)
    {
        --limit;

        if (plseg->num_segments >= CF_PDU_MAX_SEGMENTS)
        {
            /* too many */
            CF_CODEC_SET_DONE(state);
        }
        else
        {
            CF_CFDP_DecodeSegmentRequest(state, &plseg->segments[plseg->num_segments]);
        }

        if (!CF_CODEC_IS_OK(state))
        {
            break;
        }

        /* only increment if above was successful */
        ++plseg->num_segments;
    }
}

}  // namespace Ccsds
}  // namespace Svc