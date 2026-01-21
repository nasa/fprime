// ======================================================================
// \title  CfdpChunk.cpp
// \brief  CFDP chunks (sparse gap tracking) logic file
//
// This file is a port of the cf_chunks.cpp file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This class handles the complexity of sparse gap tracking so that
// the CFDP engine doesn't need to worry about it. Information is given
// to the class and when needed calculations are made internally to
// help the engine build NAK packets. Received NAK segment requests
// are stored in this class as well and used for re-transmit processing.
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

#include <string.h>

#include <Fw/Types/Assert.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>

namespace Svc {
namespace Ccsds {

void CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end)
{
    /* Sanity check */
    FW_ASSERT(end <= chunks->count, end, chunks->count);

    if (start < end)
    {
        memmove(&chunks->chunks[start], &chunks->chunks[end], sizeof(*chunks->chunks) * (chunks->count - end));
        chunks->count -= (end - start);
    }
}

void CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index)
{
    FW_ASSERT(chunks->count > 0);
    FW_ASSERT(erase_index < chunks->count, erase_index, chunks->count);

    /* to erase, move memory over the old one */
    memmove(&chunks->chunks[erase_index], &chunks->chunks[erase_index + 1],
            sizeof(*chunks->chunks) * (chunks->count - 1 - erase_index));
    --chunks->count;
}

void CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk)
{
    FW_ASSERT(chunks->count < chunks->max_chunks, chunks->count, chunks->max_chunks);
    FW_ASSERT(index_before <= chunks->count, index_before, chunks->count);

    if (chunks->count && (index_before != chunks->count))
    {
        memmove(&chunks->chunks[index_before + 1], &chunks->chunks[index_before],
                sizeof(*chunk) * (chunks->count - index_before));
    }
    memcpy(&chunks->chunks[index_before], chunk, sizeof(*chunk));

    ++chunks->count;
}

CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk)
{
    CF_ChunkIdx_t first = 0;
    CF_ChunkIdx_t i;
    CF_ChunkIdx_t count = chunks->count;
    CF_ChunkIdx_t step;

    while (count > 0)
    {
        i    = first;
        step = count / 2;
        i += step;
        if (chunks->chunks[i].offset < chunk->offset)
        {
            first = i + 1;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return first;
}

int CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    CF_Chunk_t *     prev;
    CF_ChunkOffset_t prev_end;
    CF_ChunkOffset_t chunk_end;
    int              ret = 0;

    FW_ASSERT(i <= chunks->max_chunks, i, chunks->max_chunks);

    /* Only need to check if there is a previous */
    if (i > 0)
    {
        chunk_end = chunk->offset + chunk->size;
        prev      = &chunks->chunks[i - 1];
        prev_end  = prev->offset + prev->size;

        /* Check if start of new chunk is less than end of previous (overlaps) */
        if (chunk->offset <= prev_end)
        {
            /* When combining, use the bigger of the two endings */
            if (prev_end < chunk_end)
            {
                /* Combine with previous chunk */
                prev->size = chunk_end - prev->offset;
            }
            ret = 1;
        }
    }
    return ret;
}

bool CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    CF_ChunkIdx_t    combined_i = i;
    bool             ret        = false;
    CF_ChunkOffset_t chunk_end  = chunk->offset + chunk->size;

    /* Assert no rollover, only possible as a bug */
    FW_ASSERT(chunk_end > chunk->offset, chunk_end, chunk->offset);

    /* Determine how many can be combined */
    for (; combined_i < chunks->count; ++combined_i)
    {
        /* Advance combine index until there is a gap between end and the next offset */
        if (chunk_end < chunks->chunks[combined_i].offset)
        {
            break;
        }
    }

    /* If index advanced the range of chunks can be combined */
    if (i != combined_i)
    {
        /* End is the max of last combined chunk end or new chunk end */
        chunk_end =
            CF_Chunk_MAX(chunks->chunks[combined_i - 1].offset + chunks->chunks[combined_i - 1].size, chunk_end);

        /* Use current slot as combined entry */
        chunks->chunks[i].size   = chunk_end - chunk->offset;
        chunks->chunks[i].offset = chunk->offset;

        /* Erase the rest of the combined chunks (if any) */
        CF_Chunks_EraseRange(chunks, i + 1, combined_i);
        ret = true;
    }

    return ret;
}

CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks)
{
    CF_ChunkIdx_t i;
    CF_ChunkIdx_t smallest = 0;

    for (i = 1; i < chunks->count; ++i)
    {
        if (chunks->chunks[i].size < chunks->chunks[smallest].size)
        {
            smallest = i;
        }
    }

    return smallest;
}

void CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    CF_ChunkIdx_t smallest_i;
    CF_Chunk_t *  smallest_c;
    int           n = CF_Chunks_CombineNext(chunks, i, chunk);
    int           combined;

    if (n)
    {
        combined = CF_Chunks_CombinePrevious(chunks, i, &chunks->chunks[i]);
        if (combined)
        {
            CF_Chunks_EraseChunk(chunks, i);
        }
    }
    else
    {
        combined = CF_Chunks_CombinePrevious(chunks, i, chunk);
        if (!combined)
        {
            if (chunks->count < chunks->max_chunks)
            {
                CF_Chunks_InsertChunk(chunks, i, chunk);
            }
            else
            {
                smallest_i = CF_Chunks_FindSmallestSize(chunks);
                smallest_c = &chunks->chunks[smallest_i];
                if (smallest_c->size < chunk->size)
                {
                    CF_Chunks_EraseChunk(chunks, smallest_i);
                    CF_Chunks_InsertChunk(chunks, CF_Chunks_FindInsertPosition(chunks, chunk), chunk);
                }
            }
        }
    }
}

void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size)
{
    const CF_Chunk_t    chunk = {offset, size};
    const CF_ChunkIdx_t i     = CF_Chunks_FindInsertPosition(chunks, &chunk);

    /* PTFO: files won't be so big we need to gracefully handle overflow,
     * and in that case the user should change everything in chunks
     * to use 64-bit numbers */
    FW_ASSERT((offset + size) >= offset, offset, size);

    CF_Chunks_Insert(chunks, i, &chunk);
}

void CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size)
{
    CF_Chunk_t *chunk = &chunks->chunks[0]; /* front is always 0 */

    if (size > chunk->size)
    {
        size = chunk->size;
    }
    chunk->size -= size;

    if (!chunk->size)
    {
        CF_Chunks_EraseChunk(chunks, 0);
    }
    else
    {
        chunk->offset += size;
    }
}

const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks)
{
    return chunks->count ? &chunks->chunks[0] : NULL;
}

void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t max_chunks, CF_Chunk_t *chunks_mem)
{
    FW_ASSERT(max_chunks > 0);
    chunks->max_chunks = max_chunks;
    chunks->chunks     = chunks_mem;
    CF_ChunkListReset(chunks);
}

void CF_ChunkListReset(CF_ChunkList_t *chunks)
{
    chunks->count = 0;
    memset(chunks->chunks, 0, sizeof(*chunks->chunks) * chunks->max_chunks);
}

U32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque)
{
    U32           ret = 0;
    CF_ChunkIdx_t    i   = 0;
    CF_ChunkOffset_t next_off;
    CF_ChunkOffset_t gap_start;
    CF_Chunk_t       chunk;

    FW_ASSERT(total); /* does it make sense to have a 0 byte file? */
    FW_ASSERT(start < total, start, total);

    /* simple case: there is no chunk data, which means there is a single gap of the entire size */
    if (!chunks->count)
    {
        chunk.offset = 0;
        chunk.size   = total;
        if (compute_gap_fn)
        {
            compute_gap_fn(chunks, &chunk, opaque);
        }
        ret = 1;
    }
    else
    {
        /* Handle initial gap if needed */
        if (start < chunks->chunks[0].offset)
        {
            chunk.offset = start;
            chunk.size   = chunks->chunks[0].offset - start;
            if (compute_gap_fn)
            {
                compute_gap_fn(chunks, &chunk, opaque);
            }
            ret = 1;
        }

        while ((ret < max_gaps) && (i < chunks->count))
        {
            next_off  = (i == (chunks->count - 1)) ? total : chunks->chunks[i + 1].offset;
            gap_start = (chunks->chunks[i].offset + chunks->chunks[i].size);

            chunk.offset = (gap_start > start) ? gap_start : start;
            chunk.size   = (next_off - chunk.offset);

            if (gap_start >= total)
            {
                break;
            }
            else if (start < next_off)
            {
                /* Only report if gap finishes after start */
                if (compute_gap_fn)
                {
                    compute_gap_fn(chunks, &chunk, opaque);
                }
                ++ret;
            }
            ++i;
        }
    }

    return ret;
}

}  // namespace Ccsds
}  // namespace Svc
