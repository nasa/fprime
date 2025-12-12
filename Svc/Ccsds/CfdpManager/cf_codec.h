/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * CFDP protocol data structure encode/decode API declarations
 */

#ifndef CF_CODEC_H
#define CF_CODEC_H

#include "cfe.h"
#include "cf_cfdp_pdu.h"
#include "cf_logical_pdu.h"

/**
 * @brief Tracks the current state of an encode or decode operation
 *
 * This encapsulates the common state between encode and decode
 */
typedef struct CF_CodecState
{
    bool   is_valid;    /**< \brief whether decode is valid or not.  Set false on end of decode or error condition. */
    size_t next_offset; /**< \brief Offset of next byte to encode/decode, current position in PDU */
    size_t max_size;    /**< \brief Maximum number of bytes in the PDU */
} CF_CodecState_t;

/**
 * @brief Current state of an encode operation
 *
 * State structure for encodes
 */
typedef struct CF_EncoderState
{
    CF_CodecState_t codec_state; /**< \brief Common state */
    uint8 *         base;        /**< \brief Pointer to start of encoded PDU data */
} CF_EncoderState_t;

/**
 * @brief Current state of a decode operation
 *
 * State structure for decodes
 */
typedef struct CF_DecoderState
{
    CF_CodecState_t codec_state; /**< \brief Common state */
    const uint8 *   base;        /**< \brief Pointer to start of encoded PDU data */
} CF_DecoderState_t;

/*********************************************************************************
 *
 *   GENERAL UTILITY FUNCTIONS
 *   These functions and macros support the encode/decode API
 *
 *********************************************************************************/

/************************************************************************/
/**
 * @brief Checks if the codec is currently valid or not
 *
 * @param state   Encoder/Decoder common state
 * @retval true   If encoder/decoder is still valid, has not reached end of PDU
 * @retval false  If encoder/decoder is not valid, has reached end of PDU or an error occurred
 */
static inline bool CF_CFDP_CodecIsOK(const CF_CodecState_t *state)
{
    return state->is_valid;
}

/************************************************************************/
/**
 * @brief Sets a codec to the "done" state
 *
 * This may mean end of PDU data is reached, or that an error occurred
 *
 * @param state   Encoder/Decoder common state
 */
static inline void CF_CFDP_CodecSetDone(CF_CodecState_t *state)
{
    state->is_valid = false;
}

/************************************************************************/
/**
 * @brief Obtains the current position/offset within the PDU
 *
 * @param state   Encoder/Decoder common state
 * @return Current offset in PDU
 */
static inline size_t CF_CFDP_CodecGetPosition(const CF_CodecState_t *state)
{
    return state->next_offset;
}

/************************************************************************/
/**
 * @brief Obtains the maximum size of the PDU being encoded/decoded
 *
 * @param state   Encoder/Decoder common state
 * @return Maximum size of PDU
 */
static inline size_t CF_CFDP_CodecGetSize(const CF_CodecState_t *state)
{
    return state->max_size;
}

/************************************************************************/
/**
 * @brief Obtains the remaining size of the PDU being encoded/decoded
 *
 * @param state   Encoder/Decoder common state
 * @return Remaining size of PDU
 */
static inline size_t CF_CFDP_CodecGetRemain(const CF_CodecState_t *state)
{
    return (state->max_size - state->next_offset);
}

/************************************************************************/
/**
 * @brief Resets a codec state
 *
 * @param state    Encoder/Decoder common state
 * @param max_size Maximum size of PDU
 */
static inline void CF_CFDP_CodecReset(CF_CodecState_t *state, size_t max_size)
{
    state->is_valid    = true;
    state->next_offset = 0;
    state->max_size    = max_size;
}

/************************************************************************/
/**
 * @brief Advances the position by the indicated size, confirming the block will fit into the PDU
 *
 * On encode, this confirms there is enough available space to hold a block
 * of the indicated size.  On decode, this confirms that decoding the indicated
 * number of bytes will not read beyond the end of data.
 *
 * If true, then the current position/offset is advanced by the indicated number of bytes
 * If false, then the error flag is set, so that future calls to CF_CFDP_CodecIsOK will
 * also return false.
 *
 * @note The error flag is sticky, meaning that if any encode/decode operation fails,
 * all future encode/decode requests on the same state will also fail.  Each encode/decode
 * step must check the flag, and skip the operation if it is false.  Reporting the error
 * can be deferred to the final stage, and only done once.
 *
 * @param state      Encoder/Decoder common state
 * @param chunksize  Size of next block to encode/decode
 * @retval true      If encode/decode is possible, enough space exists
 * @retval false     If encode/decode is not possible, not enough space or prior error occurred
 */
bool CF_CFDP_CodecCheckSize(CF_CodecState_t *state, size_t chunksize);

/************************************************************************/
/**
 * @brief Encode a block of data into the PDU
 *
 * Adds/Reserves space for a block of the given size in the current PDU
 *
 * @param state      Encoder state object
 * @param chunksize  Size of block to encode
 * @return Pointer to block, if successful
 * @retval NULL if not successful (no space or other error).
 */
void *CF_CFDP_DoEncodeChunk(CF_EncoderState_t *state, size_t chunksize);

/************************************************************************/
/**
 * @brief Decode a block of data from the PDU
 *
 * Deducts space for a block of the given size from the current PDU
 *
 * @param state      Decoder state object
 * @param chunksize  Size of block to decode
 * @return Pointer to block, if successful
 * @retval NULL if not successful (no space or other error).
 */
const void *CF_CFDP_DoDecodeChunk(CF_DecoderState_t *state, size_t chunksize);

/************************************************************************/
/**
 * @brief Macro to encode a block of a given CFDP type into a PDU
 *
 * This is a wrapper around CF_CFDP_DoEncodeChunk() to encode the given data type,
 * rather than a generic size.  The sizeof() the type should reflect the _encoded_
 * size within the PDU.  Specifically, this must only be used with the "CFDP" data
 * types which are specifically designed to match the binary layout of the CFDP-defined
 * header structures.
 *
 * @param state      Encoder state object
 * @param type       Data type to encode, from cf_cfdp_pdu.h
 * @return Pointer to block, if successful
 * @retval NULL if not successful (no space or other error).
 */
#define CF_ENCODE_FIXED_CHUNK(state, type) ((type *)CF_CFDP_DoEncodeChunk(state, sizeof(type)))

/************************************************************************/
/**
 * @brief Macro to decode a block of a given CFDP type into a PDU
 *
 * This is a wrapper around CF_CFDP_DoDecodeChunk() to encode the given data type,
 * rather than a generic size.  The sizeof() the type should reflect the _encoded_
 * size within the PDU.  Specifically, this must only be used with the "CFDP" data
 * types which are specifically designed to match the binary layout of the CFDP-defined
 * header structures.
 *
 * @param state      Decoder state object
 * @param type       Data type to decode, from cf_cfdp_pdu.h
 * @return Pointer to block, if successful
 * @retval NULL if not successful (no space or other error).
 */
#define CF_DECODE_FIXED_CHUNK(state, type) ((const type *)CF_CFDP_DoDecodeChunk(state, sizeof(type)))

/************************************************************************/
/**
 * @brief Macro wrapper around CF_CFDP_CodecIsOK()
 *
 * Checks the state of either an encoder or decoder object
 * This just simplifies the code, as same macro may be used with either
 * an CF_EncoderState_t or CF_DecoderState_t object.
 *
 * @param s Encoder or Decoder state
 */
#define CF_CODEC_IS_OK(s) (CF_CFDP_CodecIsOK(&((s)->codec_state)))

/************************************************************************/
/**
 * @brief Macro wrapper around CF_CFDP_CodecSetDone()
 *
 * Sets the state of either an encoder or decoder object
 * This just simplifies the code, as same macro may be used with either
 * an CF_EncoderState_t or CF_DecoderState_t object.
 *
 * @param s Encoder or Decoder state
 */
#define CF_CODEC_SET_DONE(s) (CF_CFDP_CodecSetDone(&((s)->codec_state)))

/************************************************************************/
/**
 * @brief Macro wrapper around CF_CFDP_CodecGetPosition()
 *
 * Checks the position of either an encoder or decoder object
 * This just simplifies the code, as same macro may be used with either
 * an CF_EncoderState_t or CF_DecoderState_t object.
 *
 * @param s Encoder or Decoder state
 */
#define CF_CODEC_GET_POSITION(s) (CF_CFDP_CodecGetPosition(&((s)->codec_state)))

/************************************************************************/
/**
 * @brief Macro wrapper around CF_CFDP_CodecGetRemain()
 *
 * Checks the remainder of either an encoder or decoder object
 * This just simplifies the code, as same macro may be used with either
 * an CF_EncoderState_t or CF_DecoderState_t object.
 *
 * @param s Encoder or Decoder state
 */
#define CF_CODEC_GET_REMAIN(s) (CF_CFDP_CodecGetRemain(&((s)->codec_state)))

/************************************************************************/
/**
 * @brief Macro wrapper around CF_CFDP_CodecGetSize()
 *
 * Checks the size of either an encoder or decoder object
 * This just simplifies the code, as same macro may be used with either
 * an CF_EncoderState_t or CF_DecoderState_t object.
 *
 * @param s Encoder or Decoder state
 */
#define CF_CODEC_GET_SIZE(s) (CF_CFDP_CodecGetSize(&((s)->codec_state)))

/************************************************************************/
/**
 * @brief Gets the minimum number of octets that the given integer may be encoded in
 *
 * Based on the integer value, this computes the minimum number of bytes that must be
 * allocated to that integer within a CFDP PDU.  This is typically used for entity
 * IDs and sequence numbers, where CFDP does not specify a specific size for these
 * items.  They may be encoded between 1 and 8 bytes, depending on the actual value
 * is.
 *
 * @param Value  Integer value that needs to be encoded
 * @returns Minimum number of bytes that the value requires (between 1 and 8, inclusive)
 */
uint8 CF_CFDP_GetValueEncodedSize(uint64 Value);

/************************************************************************/
/**
 * @brief Encodes the given integer value in the given number of octets
 *
 * This encodes an integer value in the specified number of octets.
 * Use CF_CFDP_GetValueEncodedSize() to determine the minimum number of octets required
 * for a given value.  Using more than the minimum is OK, but will consume extra bytes.
 *
 * @warning This function does not error check the encode_size parameter, and will encode the
 * size given, even if it results in an invalid value.  Using fewer octets than the
 * minimum reported by CF_CFDP_GetValueEncodedSize() will likely result in incorrect decoding
 * at the receiver.
 *
 * @sa CF_DecodeIntegerInSize() for the inverse operation
 *
 * @param state  Encoder state object
 * @param value  Integer value that needs to be encoded
 * @param encode_size  Number of octets to encode the value in (between 1 and 8, inclusive)
 */
void CF_EncodeIntegerInSize(CF_EncoderState_t *state, uint64 value, uint8 encode_size);

/************************************************************************/
/**
 * @brief Decodes an integer value from the specified number of octets
 *
 * This decodes an integer value in the specified number of octets.  The actual number of
 * octets must be determined using another field in the PDU before calling this function.
 *
 * @warning This function will decode exactly the given number of octets.  If this does not
 * match actual encoded size, the return value will be wrong, and it will likely also
 * throw off the decoding of any fields that follow this one.
 *
 * @sa CF_EncodeIntegerInSize() for the inverse operation
 *
 * @param state  Encoder state object
 * @param decode_size  Number of octets that the value is encoded in (between 1 and 8, inclusive)
 * @returns Decoded value
 */
uint64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, uint8 decode_size);

/*********************************************************************************
 *
 *   ENCODE API
 *
 *********************************************************************************/

/************************************************************************/
/**
 * @brief Encodes a CFDP PDU base header block, bypassing the size field
 *
 * On transmit side, the common/base header must be encoded in two parts, to deal
 * with the "total_size" field.  The initial encoding of the basic fields is
 * done as soon as it is known that a PDU of this type needs to be sent, but the
 * total size may not be yet known, as it depends on the remainder of encoding
 * and any additional data that might get added to the variable length sections.
 *
 * This function encodes all base header fields _except_ total length.  There is a
 * separate function later to update the total_length to the correct value once the
 * remainder of encoding is done.  Luckily, the total_length is in the first fixed
 * position binary blob so it is easy to update later.
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @sa CF_CFDP_EncodeHeaderFinalSize() for updating the length field once it is known
 *
 * @param state  Encoder state object
 * @param plh    Pointer to logical PDU header data
 */
void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh);

/************************************************************************/
/**
 * @brief Updates an already-encoded PDU base header block with the final PDU size
 *
 * This function encodes the "data_encoded_length" field from the logical PDU structure
 * into the encoded header block.  The PDU will also be closed (set done) to indicate that
 * no more data should be added.
 *
 * @note Unlike other encode operations, this function does not add any new blocks to the
 * PDU.  It only updates the already-encoded block at the beginning of the PDU, which must
 * have been done by a prior call to CF_CFDP_EncodeHeaderWithoutSize().
 *
 * @sa CF_CFDP_EncodeHeaderWithoutSize() for initially encoding the PDU header block
 *
 * @param state  Encoder state object
 * @param plh    Pointer to logical PDU header data
 */
void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh);

/************************************************************************/
/**
 * @brief Encodes a CFDP file directive header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param pfdir  Pointer to logical PDU file directive header data
 */
void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);

/************************************************************************/
/**
 * @brief Encodes a single CFDP Length+Value (LV) pair
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param pllv   Pointer to logical PDU LV header data
 */
void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv);

/************************************************************************/
/**
 * @brief Encodes a single CFDP Type+Length+Value (TLV) tuple
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @note Only the CF_CFDP_TLV_TYPE_ENTITY_ID TLV type is currently supported by this function,
 * but other TLV types may be added in future versions as needed.
 *
 * @param state  Encoder state object
 * @param pltlv  Pointer to single logical PDU TLV header data
 */
void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv);

/************************************************************************/
/**
 * @brief Encodes a single CFDP Segment Request block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param plseg  Pointer to single logical PDU segment request header data
 */
void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg);

/************************************************************************/
/**
 * @brief Encodes a list of CFDP Type+Length+Value tuples
 *
 * This invokes CF_CFDP_EncodeTLV() for all TLV values in the given list.
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param pltlv  Pointer to logical PDU TLV header data
 */
void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv);

/************************************************************************/
/**
 * @brief Encodes a list of CFDP Segment Request blocks
 *
 * This invokes CF_CFDP_EncodeSegmentRequest() for all segments in the given list.
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param plseg  Pointer to logical PDU segment request header data
 */
void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg);

/************************************************************************/
/**
 * @brief Encodes a CFDP Metadata (MD) header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @note this encode includes the LV pairs for source and destination file names, which are
 * logically part of the overall MD block.
 *
 * @param state  Encoder state object
 * @param plmd   Pointer to logical PDU metadata header data
 */
void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd);

/************************************************************************/
/**
 * @brief Encodes a CFDP File Data (FD) header block
 *
 * This only encodes the FD header fields, specifically the data offset (required) and any
 * metadata fields, if indicated.  This does _not_ encode any actual file data.
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param with_meta Whether to include optional continuation and segment request fields (always false currently)
 * @param plfd   Pointer to logical PDU file header data
 */
void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd);

/************************************************************************/
/**
 * @brief Encodes a CFDP End-of-File (EOF) header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @note this encode includes any TLV values which are indicated in the logical data structure
 *
 * @param state  Encoder state object
 * @param pleof Pointer to logical PDU EOF header data
 */
void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof);

/************************************************************************/
/**
 * @brief Encodes a CFDP Final (FIN) header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @note this encode includes any TLV values which are indicated in the logical data structure
 *
 * @param state  Encoder state object
 * @param plfin  Pointer to logical PDU FIN header data
 */
void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin);

/************************************************************************/
/**
 * @brief Encodes a CFDP Acknowledge (ACK) header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param plack  Pointer to logical PDU ACK header data
 */
void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack);

/************************************************************************/
/**
 * @brief Encodes a CFDP Non-Acknowledge (NAK) header block
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @note this encode includes any Segment Request values which are indicated in the logical data structure
 *
 * @param state  Encoder state object
 * @param plnak  Pointer to logical PDU NAK header data
 */
void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak);

/************************************************************************/
/**
 * @brief Encodes a CFDP CRC/Checksum
 *
 * The data in the logical header will be appended to the encoded PDU at the current position
 *
 * If the encoder is in an error state, nothing is encoded, and the state of the
 * encoder is not changed.
 *
 * @param state  Encoder state object
 * @param plcrc  Pointer to logical CRC value
 */
void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, uint32 *plcrc);

/*********************************************************************************
 *
 *   DECODE API
 *
 *********************************************************************************/

/************************************************************************/
/**
 * @brief Decodes a CFDP base PDU header
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @note On decode the entire base header is decoded in a single call, the size
 * will be decoded like any other field.
 *
 * @param state  Decoder state object
 * @param plh    Pointer to logical PDU base header data
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @retval CF_ERROR on error.
 */
CFE_Status_t CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh);

/************************************************************************/
/**
 * @brief Decodes a CFDP file directive header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param pfdir  Pointer to logical PDU file directive header data
 */
void CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);

/************************************************************************/
/**
 * @brief Decodes a single CFDP Length+Value (LV) pair
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param pllv   Pointer to single logical PDU LV data
 */
void CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv);

/************************************************************************/
/**
 * @brief Decodes a single CFDP Type+Length+Value (TLV) tuple
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param pltlv  Pointer to single logical PDU TLV data
 */
void CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv);

/************************************************************************/
/**
 * @brief Decodes a single CFDP Segment Request block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plseg  Pointer to single logical PDU segment request header data
 */
void CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg);

/************************************************************************/
/**
 * @brief Decodes a list of CFDP Type+Length+Value tuples
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param pltlv  Pointer to logical PDU TLV header data
 * @param limit  Maximum number of TLV objects to decode
 */
void CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, uint8 limit);

/************************************************************************/
/**
 * @brief Decodes a list of CFDP Segment Request blocks
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plseg  Pointer to logical PDU segment request header data
 * @param limit  Maximum number of Segment Request objects to decode
 */
void CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, uint8 limit);

/************************************************************************/
/**
 * @brief Decodes a CFDP Metadata (MD) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plmd   Pointer to logical PDU metadata header data
 */
void CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd);

/************************************************************************/
/**
 * @brief Decodes a CFDP File Data (FD) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param with_meta Whether to include optional continuation and segment request fields (always false currently)
 * @param plfd   Pointer to logical PDU file header data
 */
void CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd);

/************************************************************************/
/**
 * @brief Decodes a CFDP End-of-File (EOF) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param pleof Pointer to logical PDU EOF header data
 */
void CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof);

/************************************************************************/
/**
 * @brief Decodes a CFDP Final (FIN) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plfin  Pointer to logical PDU FIN header data
 */
void CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin);

/************************************************************************/
/**
 * @brief Decodes a CFDP Acknowledge (ACK) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plack  Pointer to logical PDU ACK header data
 */
void CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack);

/************************************************************************/
/**
 * @brief Decodes a CFDP Non-Acknowledge (NAK) header block
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plnak  Pointer to logical PDU NAK header data
 */
void CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak);

/************************************************************************/
/**
 * @brief Decodes a CFDP CRC/Checksum
 *
 * The data will be decoded from the encoded PDU at the current position and
 * the logical fields will be saved to the given data structure
 *
 * If the encoder is in an error state, nothing is decoded, and the state of the
 * decoder is not changed.
 *
 * @param state  Decoder state object
 * @param plcrc   Pointer to logical CRC value
 */
void CF_CFDP_DecodeCrc(CF_DecoderState_t *state, uint32 *plcrc);

#endif /* !CF_CODEC_H */
