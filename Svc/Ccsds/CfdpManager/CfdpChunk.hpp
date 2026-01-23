// ======================================================================
// \title  CfdpChunks.hpp
// \brief  CFDP chunks (spare gap tracking) header file
//
// This file is a port of the cf_chunks.h file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
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

#ifndef CFDP_CHUNK_HPP
#define CFDP_CHUNK_HPP

#include <Fw/FPrimeBasicTypes.hpp>

#include <Svc/Ccsds/Types/CfdpStatusEnumAc.hpp>

namespace Svc {
namespace Ccsds {

typedef U32 CF_ChunkIdx_t;
typedef U32 CF_ChunkOffset_t;
typedef U32 CF_ChunkSize_t;

/**
 * @brief Pairs an offset with a size to identify a specific piece of a file
 */
typedef struct CF_Chunk
{
    CF_ChunkOffset_t offset; /**< \brief The start offset of the chunk within the file */
    CF_ChunkSize_t   size;   /**< \brief The size of the chunk */
} CF_Chunk_t;

/**
 * @brief A list of CF_Chunk_t pairs
 *
 * This list is ordered by chunk offset, from lowest to highest
 */
typedef struct CF_ChunkList
{
    CF_ChunkIdx_t count;      /**< \brief number of chunks currently in the array */
    CF_ChunkIdx_t max_chunks; /**< \brief maximum number of chunks allowed in the list (allocation size) */
    CF_Chunk_t *  chunks;     /**< \brief chunk list array */
} CF_ChunkList_t;

/**
 * @brief Function for use with CF_ChunkList_ComputeGaps()
 *
 * @param cs Pointer to the CF_ChunkList_t object
 * @param chunk  Pointer to the chunk being currently processed
 * @param opaque Opaque pointer passed through from initial call
 */
typedef void (*CF_ChunkList_ComputeGapFn_t)(const CF_ChunkList_t *cs, const CF_Chunk_t *chunk, void *opaque);

/**
 * @brief Selects the larger of the two passed-in offsets
 *
 * @param a First chunk offset
 * @param b Second chunk offset
 * @return the larger CF_ChunkOffset_t value
 */
static inline CF_ChunkOffset_t CF_Chunk_MAX(CF_ChunkOffset_t a, CF_ChunkOffset_t b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

/************************************************************************/
/** @brief Initialize a CF_ChunkList_t structure.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL. chunks_mem must not be NULL.
 *
 * @param chunks      Pointer to CF_ChunkList_t object to initialize
 * @param max_chunks  Maximum number of entries in the chunks_mem array
 * @param chunks_mem  Array of CF_Chunk_t objects with length of max_chunks
 */
void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t max_chunks, CF_Chunk_t *chunks_mem);

/************************************************************************/
/** @brief Public function to add a chunk.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @param chunks   Pointer to CF_ChunkList_t object
 * @param offset   Offset of chunk to add
 * @param size     Size of chunk to add
 */
void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size);

/************************************************************************/
/** @brief Resets a chunks structure.
 *
 * All chunks are removed from the list, but the max_chunks and chunk memory
 * pointers are retained.  This returns the chunk list to the same state as
 * it was after the initial call to CF_ChunkListInit().
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @param chunks   Pointer to CF_ChunkList_t object
 */
void CF_ChunkListReset(CF_ChunkList_t *chunks);

/************************************************************************/
/** @brief Public function to remove some amount of size from the first chunk.
 *
 * @par Description
 *       This may remove the chunk entirely. This function is to satisfy the
 *       use-case where data is retrieved from the structure in-order and
 *       once consumed should be removed.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL and list must not be empty
 *
 * @note
 * Good computer science would have a generic remove function, but that's much more complex
 * than we need for the use case. We aren't trying to make chunks a general purpose
 * reusable module, so just take the simple case that we need.
 *
 * @param chunks   Pointer to CF_ChunkList_t object
 * @param size     Size to remove
 */
void CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size);

/************************************************************************/
/** @brief Public function to get the entire first chunk from the list
 *
 * This returns the first chunk from the list, or NULL if the list is empty.
 *
 * @note The chunk remains on the list - this call does not consume or remove the chunk
 * from the list.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @returns Pointer to first chunk from the CF_ChunkList_t object
 * @retval  NULL if the list was empty
 */
const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks);

/************************************************************************/
/** @brief Compute gaps between chunks, and call a callback for each.
 *
 * @par Description
 *       This function walks over all chunks and computes the gaps between.
 *       It can exit early if the calculated gap start is larger than the
 *       desired total.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL. compute_gap_fn is a valid function address.
 *
 * @param chunks         Pointer to CF_ChunkList_t object
 * @param max_gaps       Maximum number of gaps to compute
 * @param total          Size of the entire file
 * @param start          Beginning offset for gap computation
 * @param compute_gap_fn Callback function to be invoked for each gap
 * @param opaque         Opaque pointer to pass through to callback function
 *
 * @returns The number of computed gaps.
 */
U32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque);

/************************************************************************/
/** @brief Erase a range of chunks.
 *
 * Items in the list starting at the end index will be shifted/moved to close the gap.
 * If end <= start nothing will be done (OK to pass matching start/end as a no-op)
 * If end == list size, list from start on will be erased (nothing moved)
 *
 * Example:
 *   list = {a, b, c, d, e}
 *   EraseRange index start 1, end 3
 *   list = {a, d, e}
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @param chunks   Pointer to CF_ChunkList_t object
 * @param start    Starting chunk ID to erase (inclusive)
 * @param end      Ending chunk ID (exclusive, this chunk will not be erased)
 */
void CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end);

/************************************************************************/
/** @brief Erase a single chunk.
 *
 * @note This changes the chunk IDs of all chunks that follow
 * Items in the list after the erase_index will be shifted/moved to close the gap.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @param chunks      Pointer to CF_ChunkList_t object
 * @param erase_index chunk ID to erase
 */
void CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index);

/************************************************************************/
/** @brief Insert a chunk before index_before.
 *
 * @note This changes the chunk IDs of all chunks that follow
 * Items in the list after the index_before will be shifted/moved to open a gap.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL, chunk must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 * @param index_before position to insert at - this becomes the ID of the inserted chunk
 * @param chunk        Chunk data to insert (copied)
 */
void CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk);

/************************************************************************/
/** @brief Finds where a chunk should be inserted in the chunks.
 *
 * @par Description
 *       This is a C version of std::lower_bound from C++ algorithms.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL, chunk must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 * @param chunk        Chunk data to insert
 *
 * @returns an index to the first chunk that is greater than or equal to the requested's offset.
 *
 */
CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk);

/************************************************************************/
/** @brief Possibly combines the given chunk with the previous chunk.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL, chunk must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 * @param i            Index of chunk to combine
 * @param chunk        Chunk data to combine
 *
 * @returns boolean code indicating if chunks were combined
 * @retval 1 if combined with another chunk
 * @retval 0 if not combined
 *
 */
int CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);

/************************************************************************/
/** @brief Possibly combines the given chunk with the next chunk.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL, chunk must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 * @param i            Index of chunk to combine
 * @param chunk        Chunk data to combine
 *
 * @returns boolean code indicating if chunks were combined
 * @retval true if combined with another chunk
 * @retval false if not combined
 *
 */
bool CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);

/************************************************************************/
/** @brief Finds the smallest size out of all chunks.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 *
 * @returns The chunk index with the smallest size.
 * @retval  0 if the chunk list is empty
 *
 */
CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks);

/************************************************************************/
/** @brief Insert a chunk.
 *
 * @par Description
 *       Inserts the chunk at the specified location. May combine with
 *       an existing chunk if contiguous.
 *
 * @par Assumptions, External Events, and Notes:
 *       chunks must not be NULL, chunk must not be NULL.
 *
 * @param chunks       Pointer to CF_ChunkList_t object
 * @param i            Position to insert chunk at
 * @param chunk        Chunk data to insert
 */
void CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_CHUNK_HPP */
